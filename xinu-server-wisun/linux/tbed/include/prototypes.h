extern int ts_find();
extern void help();
extern int srbit (byte addr[], int, int);
extern int isnumeric(char *);
extern struct c_msg command_handler(char[]);
extern int makeConnection(char *, char *, char *);
extern void mapping_list(char *); 
extern void topodump(struct c_msg *);
extern void error_handler(char *);
extern void ping_reply_handler(struct c_msg *);
extern void pingall_handler(struct c_msg *);
extern void response_handler(struct c_msg *);
extern void error_handler(char *);
extern void udp_process(const char *, char *);
extern char * make_result_file(char *);
extern void cleanup(char *);
extern void getmap(struct c_msg *);
extern void ping6();
extern void init_maclist();
