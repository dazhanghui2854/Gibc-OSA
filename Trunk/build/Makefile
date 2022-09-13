#*******************************************************************************
# Makefile
#
#
# Author : Zhang Hui <446387817@qq.com>
# Version: V1.0.0  2021-09-01 Create
#
# Description: Makefile for compiling OSA.
#
# Modification:
# 1. Date     :
#    Revision :
#    Author   :
#    Contents :
#
#******************************************************************************/
PWD:=$(shell pwd)


include $(PWD)/Rules.mk


##目标文件
LIB_NAME := libosa.a

##demo用例
DEMO_NAME := app-test

%.o: %.c
	@$(CC) -o $@ -c $< $(CFLAGS)




.PHONY: all clean

all: kernel_lib


kernel_lib:$(SRC_OBJS)
	mkdir -p $(BIN_PATH)/debug
	mkdir -p $(BIN_PATH)/release
	@$(AR) -r $(BIN_PATH)/debug/$(LIB_NAME) $(SRC_OBJS)
	@$(CP) -r $(BIN_PATH)/debug/$(LIB_NAME) $(BIN_PATH)/release
	@$(STRIP) -s $(BIN_PATH)/release/$(LIB_NAME)
	

clean:
	@$(RM) -rf $(BIN_PATH)/*
	@$(RM) -rf $(SRC_OBJS)

help:
	@echo "make plat=x86"
	@echo "make plat=Hi3559"



