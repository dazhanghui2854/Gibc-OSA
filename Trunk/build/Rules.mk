#*******************************************************************************
# Rules.make
#
# Author : Zhang Hui <446387817@qq.com>
#
# Version: V1.0.0  2021-09-1 Create
#
# Description: Rules for makefile
#
# Modification: 
# 1. Date     : 
#    Revision : 
#    Author   : 
#    Contents : 
#
#*******************************************************************************

ifeq ($(plat),x86)
	CROSS:=
else ifeq ($(plat),hi3559a)
	CROSS:=

endif
	
CC  	= @echo " GCC  		 $@" ; $(CROSS)gcc
CPP 	= @echo " G++  		 $@" ; $(CROSS)g++
LD  	= @echo " LD   		 $@" ; $(CROSS)ld
AR  	= @echo " AR   		 $@" ; $(CROSS)ar
STRIP   = @echo " STRIP   	 $@" ; $(CROSS)strip
RM	  	= @echo " RM *.o *.a" 	 ; rm -rf
CP    	= @echo " CP   		 $@" ; cp -rf



##三方库路径，后续有需要添加
CFLAGS += -DOS_LINUX


##编译宏


##系统库
CFLAGS += -lpthread

##编译报警选项

CFLAGS += -Wall


##debug or release
ifeq ($(DEBUG),1)
CFLAGS += -gcc
CFLAGS += -O0
else
CFLAGS += -O2
endif

##asan工具配置






ifndef PWD
PWD   := $(shell pwd)
endif





SRC_PATH:= $(PWD)/../src
INC_PATH:= $(PWD)/../inc
TARGET := libosa.a

ROOT_DIR := $(PWD)/..
BIN_DIR  := $(ROOT_DIR)/bin/$(plat)/

SRC = $(wildcard $(SRC_PATH)/*.c)
OBJ = $(patsubst $(SRC_PATH)/%.c, $(SRC_PATH)/%.o, $(SRC))


CFLAGS:=
CFLAGS+= -g
#CFLAGS+= -pthread
##头文件路径
CFLAGS+= -I$(INC_PATH)
CFLAGS+= -I$(INC_PATH)/inc_sys/common
CFLAGS+= -I$(INC_PATH)/inc_sys/common/osa


LDFLAGS:= -pthread
#LDFLAGS+= -lz
#LDFLAGS+= -lm
#LDFLAGS+= -lbz2
#库的位置L和名称l都要加入LDFLAGS
LDFLAGS+= -std=c++11


PRO_PATH?=$(PWD)/..

BUD_PATH?=$(PRO_PATH)/build

##头文件路径
INC_PATH?=$(PRO_PATH)/include

##源文件目录
SRC_PATH?=$(PRO_PATH)/src

##目标文件路径
BIN_PATH?=$(PRO_PATH)/bin




##待编译.c源文件
cur_src=$(wildcard $(SRC_PATH)/*.c)


##目标文件
SRC_OBJS = $(cur_src:%.c=%.o)




##三方链接库





