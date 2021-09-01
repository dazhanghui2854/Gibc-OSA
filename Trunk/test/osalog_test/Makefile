
ifndef PWD           
PWD   := $(shell pwd)
endif
 
ROOT_DIR := $(PWD)/../../
BIN_DIR  := $(ROOT_DIR)/bin/$(plat)/

LINUX_DIR := $(ROOT_DIR)/src/os/linux
KERNEL_DIR := $(LINUX_DIR)/kermod
USER_DIR   := $(LINUX_DIR)/usrmod
 
ifeq ($(plat),hi3559a)
	CROSS_COMPILE := aarch64-hisiv610-linux-gnu-
endif

SVN_NUM := $(shell svn info $(ROOT_DIR) \
                  | sed -n '/.*-*:*:*+./{g;1!p;};h' \
                  | grep -oP "\d+")

OSA_MODULE_NAME:= OSA_TESTU@$(SVN_NUM)

CC := $(CROSS_COMPILE)gcc
AR := $(CROSS_COMPILE)ar
STRIP := $(CROSS_COMPILE)strip

CFLAGS := -g -I$(ROOT_DIR)/inc -I$(ROOT_DIR)/inc/inc_sys/common/osa/ -I$(ROOT_DIR)/inc/inc_sys/common/
CFLAGS += -DOSA_MODULE_NAME='"$(OSA_MODULE_NAME)"'

LDFLAGS := -static -losa -L$(BIN_DIR) -lpthread

OBJS = osa_test.o

BIN = osaLogCtl

$(BIN): $(OBJS)
	echo $(CFLAGS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	$(STRIP) $@

clean:
	rm *.o -rf
	rm $(BIN) -rf
