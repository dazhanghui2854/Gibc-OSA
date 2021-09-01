#*******************************************************************************
# Makefile
#
# Copyright (C) 2011-2013 ZheJiang Dahua Technology CO.,LTD.
#
# Author : Zhang Hui <446387817@qq.com>
# Version: V1.0.0  2021-09-01 Create
#
# Description: Makefile for compiling OSA.
#              Example see help tag.
#	           Supporting OS  : linux/bios"
#              Supporting mode: kernel/user, m3/c674"
#              Supporting plat: dm36x/dm812x/a5s"
#
#              Release version Example:"
#              make os=linux mode=kernel plat=dm812x"
#              make os=bios  plat=dm812x"
#
#              Debug version Example:"
#              make os=linux mode=user   plat=dm812x debug=debug"
#              make os=bios  plat=dm812x debug=debug"

# Modification:
# 1. Date     :
#    Revision :
#    Author   :
#    Contents :
#
#******************************************************************************/


include $(PWD)/Rules.mk



TARGET: all


help:
	@echo ""
	@echo "  ERROR: You must specify os/platform or mode/debug option !"
	@echo ""
	@echo "  Supporting OS  : linux/bios"
	@echo "  Supporting mode: kernel/user, m3/c674"
	@echo "  Supporting plat: dm36x/dm812x/a5s"
	@echo ""
	@echo "  Release version Example:"
	@echo "  make os=linux mode=kernel plat=dm812x"
	@echo "  make os=bios  plat=dm812x"
	@echo ""
	@echo "  Debug version Example:"
	@echo "  make os=linux mode=user plat=dm812x debug=debug"
	@echo "  make os=bios  plat=dm812x debug=debug"
	@echo ""

clean:
	make all TAG=clean


.PHONY:
	default all install clean testx flint

export TAG
