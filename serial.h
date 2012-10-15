
void serial_init(char* ttypath);

void serial_write(char* c, int len);
void serial_writebyte(char c);
int serial_selectread();
