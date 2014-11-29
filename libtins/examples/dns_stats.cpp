/*
 * Copyright (c) 2014, Matias Fontanini
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * * Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above
 *   copyright notice, this list of conditions and the following disclaimer
 *   in the documentation and/or other materials provided with the
 *   distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <iostream>
#include <mutex>
#include <chrono>
#include <map>
#include <thread>
#include <algorithm>
#include <tins/tins.h>

using namespace Tins;

// Holds the DNS response time statistics. The response time is 
// represented using the Duration template parameter.
template<typename Duration>
class statistics {
public:
    using duration_type = Duration;
    using locker_type = std::lock_guard<std::mutex>;
    
    struct information {
        duration_type average, worst;
        size_t count;
    };
    
    statistics()
    : m_duration(), m_worst(duration_type::min()), m_count()
    {
        
    }
    
    void add_response_time(const duration_type& duration)
    {
        locker_type _(m_lock);
        m_duration += duration;
        m_count++;
        m_worst = std::max(m_worst, duration);
    }
    
    information get_information() const 
    {
        locker_type _(m_lock);
        if(m_count == 0)
            return { };
        else 
            return { m_duration / m_count, m_worst, m_count };
    };
private:
    duration_type m_duration, m_worst;
    size_t m_count;
    mutable std::mutex m_lock;
};

// Sniffs and tracks DNS queries. When a matching DNS response is found,
// the response time is added to a statistics object.
//
// This class performs *no cleanup* on data associated with queries that
// weren't answered.
class dns_monitor {
public:
    // The response times are measured in milliseconds
    using duration_type = std::chrono::milliseconds;
    // The statistics type used.
    using statistics_type = statistics<duration_type>;
    
    void run(BaseSniffer& sniffer);
    const statistics_type& stats() const {
        return m_stats;
    }
private:
    using packet_info = std::tuple<IPv4Address, IPv4Address, uint16_t>;
    using clock_type = std::chrono::steady_clock;
    using time_point_type = std::chrono::time_point<clock_type>;

    bool callback(const PDU& pdu);
    static packet_info make_packet_info(const PDU& pdu, const DNS& dns);
    
    statistics_type m_stats;    
    std::map<packet_info, time_point_type> m_packet_info;
};

void dns_monitor::run(BaseSniffer& sniffer)
{
    sniffer.sniff_loop(
        std::bind(
            &dns_monitor::callback, 
            this, 
            std::placeholders::_1
        )
    );
}

bool dns_monitor::callback(const PDU& pdu)
{
    auto now = clock_type::now();
    auto dns = pdu.rfind_pdu<RawPDU>().to<DNS>();
    auto info = make_packet_info(pdu, dns);
    // If it's a query, add the sniff time to our map.
    if(dns.type() == DNS::QUERY) {
        m_packet_info.insert(
            std::make_pair(info, now)
        );
    }
    else {
        // It's a response, we need to find the query in our map.
        auto iter = m_packet_info.find(info);
        if(iter != m_packet_info.end()) {
            // We found the query, let's add the response time to the
            // statistics object.
            m_stats.add_response_time(
                std::chrono::duration_cast<duration_type>(now - iter->second)
            );
            // Forget about the query.
            m_packet_info.erase(iter);
        }
    }
    return true;
}

// It is required that we can identify packets sent and received that
// hold the same DNS id as belonging to the same query. 
// 
// This function retrieves a tuple (addr, addr, id) that will achieve it.
auto dns_monitor::make_packet_info(const PDU& pdu, const DNS& dns) -> packet_info
{
    const auto& ip = pdu.rfind_pdu<IP>();
    return std::make_tuple( 
        // smallest address first
        std::min(ip.src_addr(), ip.dst_addr()),
        // largest address second
        std::max(ip.src_addr(), ip.dst_addr()),
        dns.id()
    );
}

int main(int argc, char *argv[]) {
    if(argc != 2) {
        std::cout << "Usage: " << *argv << " <interface>\n";
        return 1;
    }
    try {
        SnifferConfiguration config;
        config.set_promisc_mode(true);
        config.set_filter("udp and dst port 53");
        Sniffer sniffer(argv[1], config);
        dns_monitor monitor;
        std::thread thread(
            [&]() {
                monitor.run(sniffer);
            }
        );
        while(true) {
            auto info = monitor.stats().get_information();
            std::cout << "\rAverage " << info.average.count() 
                        << "ms. Worst: " << info.worst.count() << "ms. Count: "
                        << info.count;
            std::cout.flush();
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    catch(std::exception& ex) {
        std::cout << "[-] Error: " << ex.what() << std::endl;
    }
}
