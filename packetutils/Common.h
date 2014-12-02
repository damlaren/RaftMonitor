/* Common definitions of the different types of packets
 * Adapters should return these. 
 */

const int GET_RPC_VERSIONS = 0; 
const int OPEN_SESSION = 1;
const int GET_CONFIG = 2;
const int SET_CONFIG = 3; 
const int READ_ONLY_TREE = 4;
const int READ_WRITE_TREE = 5;
const int REQUEST_VOTE = 6;
const int APPEND_HEARTBEAT = 7;
const int APPEND_SNAPSHOT = 8;
const int REPLY = 9;
const int INVALID = -1;