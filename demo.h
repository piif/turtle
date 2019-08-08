class DemoStream : public Stream {
  protected:
    char *code = "[000100;D;M100;T90;M100;T90;C100,90;C-100,90;T-90;M100;T-90;M100;U;];";
    char *c = code;

  public:
    int available(void) {
    	return *c != '\0';
    }
    int peek(void) {
    	return (*c == '\0') ? -1 : *c;
    }
    int read(void) {
    	if (*c == '\0') {
    		return -1;
    	} else {
    		return *c++;
    	}
    }
    virtual void flush(void) {}
    virtual size_t write(uint8_t) {}
};

DemoStream DemoProgram;