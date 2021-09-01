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
	
CC = gcc	
RM	  := rm -rf
CP    := cp
MV    := mv

ifndef PWD
PWD   := $(shell pwd)
endif





SRC_PATH:= $(PWD)/../src/
INC_PATH:= $(PWD)/../inc/
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




export ROOT_DIR
export BIN_DIR


