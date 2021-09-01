
#include <osa.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

typedef void (*FuncHandler)(void *,void *);

typedef struct 
{
	FuncHandler handler;
	const char * handlerstr;					
} HandleFuncId;

#define Handlerfunc(func, strid) {(FuncHandler) func, #strid} 

void OSA_LogManagerUser_iftest(int seed, char *modName)
{
	int ret, i;
	int level;	
	size_t len;
	Uint64 printedBytes = 0;
	
	OSA_LogManagerCommon *osa_entity = NULL;
	
	len = OSA_LogManagerGetAllEntryInfoSize();
	if (len < 0) {
		OSA_ERROR("Get_AllEntryInfoSize Failed \n");
		return;
	}	
	
	osa_entity = (OSA_LogManagerCommon *)malloc(len); 
	if (!osa_entity) {
		OSA_ERROR("No Enough mem \n");
		return;
	}
	
	memset(osa_entity, 0, len);
	
	ret = OSA_LogManagerGetAllEntryInfo(osa_entity, len);
	if (ret != 0) {
		OSA_ERROR("Get_AllEntryInfo Failed \n");
		return;
	}	

	for (i = 0; i < len / sizeof(OSA_LogManagerCommon); i++) {
		OSA_INFO("Entity %s level %d enable %d stat %d \n", osa_entity[i].osa_Name, 
			osa_entity[i].osa_LogLevel, osa_entity[i].osa_PrintEnable, osa_entity[i].osa_Stat);
			
		OSA_LogManagerGetEntryPrintStatics(osa_entity[i].osa_Name, &printedBytes);
		OSA_INFO("entiry %s printedBytes %llu \n", osa_entity[i].osa_Name, printedBytes);

		OSA_LogManagerSetEntryPrintLevel(osa_entity[i].osa_Name, ((seed + i) % OSA_LOG_LV_MAX));

		OSA_LogManagerGetEntryPrintLevel(osa_entity[i].osa_Name, &level);
		OSA_INFO("New Level %d intend %d \n", level, ((seed + i) % OSA_LOG_LV_MAX));

		if (0 == memcmp(osa_entity[i].osa_Name, modName, strlen(modName) + 1)) {
			OSA_INFO("**** Set %s  %s Enable to %d ****\n", modName, osa_entity[i].osa_Name, (seed % 2));
			OSA_LogManagerSetEntryEnable(osa_entity[i].osa_Name, (seed % 2));
		}
	}
	
	free(osa_entity);
	osa_entity = NULL;
	return;
}

void OSA_LogManagerUserRoundTest(void *p1, void *p2)
{
	int ret;
	int i = 0;	
	int osa_print = 0;
	int test_round = atoi(p1);

	OSA_LogManagerCommon LogManagerComm;
	memset(&LogManagerComm, 0, sizeof(OSA_LogManagerCommon));

	OSA_INFO("User Interface OSA_INFO Test \n");
	OSA_WARN("User Interface OSA_WARN Test \n");

	OSA_LogManagerDisableAllEntryStat(OSA_LOGMANAGER_ENABLE);

	memcpy(LogManagerComm.osa_Name, p2, OSA_LOGENTITY_NAMELEN);

	ret = OSA_LogManagerGetEntryInfo(&LogManagerComm);
	if (ret < 0) {
		OSA_ERROR("Get_AllEntryInfoSize Failed \n");
		return;
	}

	for (i = 0; i < test_round; i++) {
		OSA_INFO("Test Round %d\n", i);
		OSA_INFO("%s|%d osa_Name %s osa_Stat %d osa_LogLevel %d osa_PrintEnable %d \n", __func__, __LINE__, LogManagerComm.osa_Name, 
			LogManagerComm.osa_Stat, LogManagerComm.osa_LogLevel,LogManagerComm.osa_PrintEnable);

		OSA_LogManagerUser_iftest(i, p2);

		if (i && (i % 3 == 0)) {
			OSA_INFO("%s|%d round %d osa_Name %s osa_Stat %d osa_LogLevel %d \n", __func__, __LINE__, i, LogManagerComm.osa_Name,
				LogManagerComm.osa_Stat, LogManagerComm.osa_LogLevel);
			OSA_LogManagerDisableAllEntryStat(OSA_LOGMANAGER_DISABLE);
			osa_print = 1;
			LogManagerComm.osa_LogLevel = OSA_LOG_LV_WARN;
			LogManagerComm.osa_Stat = OSA_LOGMANAGER_ENABLE;
			ret = OSA_LogManagerSetEntryInfo(&LogManagerComm);
			if (ret < 0) {
				OSA_ERROR("Get_AllEntryInfoSize Failed \n");
				return;
			}
		}

		sleep(1);
		if (i && (i % 50 == 0) && osa_print) {
			OSA_LogManagerDisableAllEntryStat(OSA_LOGMANAGER_ENABLE);
			osa_print = 0;			
			LogManagerComm.osa_LogLevel = OSA_LOG_LV_DBG;
			LogManagerComm.osa_Stat = OSA_LOGMANAGER_DISABLE;
			ret = OSA_LogManagerSetEntryInfo(&LogManagerComm);
			if (ret < 0) {
				OSA_ERROR("Get_AllEntryInfoSize Failed \n");
				return;
			}
			OSA_INFO("%s|%d round %d %s osa_Stat %d osa_LogLevel %d \n", __func__, __LINE__, i,
				LogManagerComm.osa_Name, LogManagerComm.osa_Stat, LogManagerComm.osa_LogLevel);
		}
	}

	return;
}

void OSA_LogManagerUserGetLevel(void *p1)
{
	int ret;
	int oldLevel;
	
	if (!p1) {
        printf("OSA_LogManagerUserGetLevel Param Invalid !\n");
        return ;
    }
		
	ret = OSA_LogManagerGetEntryPrintLevel(p1, &oldLevel);
    if (OSA_SOK != ret) {
        printf("OSALogManager Get %s Levle Failed!\n", p1);
        return ;
    }

	OSA_INFO("%s osa_LogLevel %d \n", p1, oldLevel);;
	
    return ;
}

void OSA_LogManagerUserSetLevel(void *p1, void *p2)
{
	int ret;
	int oldLevel, newLevel;		
	
    if (!p1 || NULL == p2) {
         printf("OSA_LogManagerUserSetLevel Param Invalid !\n");
        return ;
    }
	
	newLevel = atoi(p2);
	
	ret = OSA_LogManagerGetEntryPrintLevel(p1, &oldLevel);
    if (OSA_SOK != ret) {
        printf("OSALogManager Get %s Levle Failed!\n", p1);
        return ;
    }	
	
	ret = OSA_LogManagerSetEntryPrintLevel(p1, newLevel);		
    if (OSA_SOK != ret) {
           OSA_ERROR("OSALogManager Get %s Levle Failed!\n", p1);
           return ;
    }
	
	OSA_INFO("%s osa_LogLevel %d set to %d \n", (char *)p1, oldLevel, newLevel);
	
    return;
}

void OSA_LogManagerUserGetEnable(void *p1)
{
	int ret;
	OSA_LogManagerCommon *pOsa_Entity = NULL;
	
	if (!p1) {
        printf("OSA_LogManagerUserGetEnable Param Invalid !\n");
        return ;
    }	
	
	pOsa_Entity = (OSA_LogManagerCommon *)malloc(sizeof(OSA_LogManagerCommon)); 
	if (!pOsa_Entity) {
		OSA_ERROR("No Enough mem \n");
		return;
	}
	
	memset(pOsa_Entity, 0, sizeof(OSA_LogManagerCommon));
	
	memcpy(pOsa_Entity->osa_Name, p1, strlen(p1));
	
	ret = OSA_LogManagerGetEntryInfo(pOsa_Entity);
    if (OSA_SOK != ret) {
        printf("OSA_LogManagerGetEntryInfo Get %s Levle Failed!\n", p1);
        return ;
    }

	OSA_INFO("%s osa_PrintEnable %d \n", p1, pOsa_Entity->osa_PrintEnable);
	
	free(pOsa_Entity);
    return ;
}

void OSA_LogManagerUserSetEnable(void *p1, void *p2)
{
	int ret;
	int oldEnable;
	OSA_LogManagerCommon *pOsa_Entity = NULL;
	
	if (!p1 || !p2) {
        printf("OSA_LogManagerUserSetEnable Param Invalid !\n");
        return ;
    }
	
	pOsa_Entity = (OSA_LogManagerCommon *)malloc(sizeof(OSA_LogManagerCommon));
	if (!pOsa_Entity) {
		OSA_ERROR("No Enough mem \n");
		return;
	}
	
	memset(pOsa_Entity, 0, sizeof(OSA_LogManagerCommon));
	
	memcpy(pOsa_Entity->osa_Name, p1, strlen(p1));
	
	ret = OSA_LogManagerGetEntryInfo(pOsa_Entity);
    if (OSA_SOK != ret) {
        printf("OSA_LogManagerGetEntryInfo Get %s Levle Failed!\n", p1);
        return ;
    }

	oldEnable = pOsa_Entity->osa_PrintEnable;

	ret = OSA_LogManagerSetEntryEnable(p1, atoi(p2));
    if (OSA_SOK != ret) {
           OSA_ERROR("OSA_LogManagerSetEntryEnable %s %d to %d Failed!\n", p1, oldEnable, atoi(p2));
           return ;
    }

	OSA_INFO("OSA_LogManagerSetEntryEnable %s %d to %d OK!\n", p1, oldEnable, atoi(p2));
	
	free(pOsa_Entity);
    return;
}

void OSA_LogManagerUserGetStat(void *p1)
{
	int ret;
	OSA_LogManagerCommon *pOsa_Entity = NULL;
	
	if (!p1) {
        printf("OSA_LogManagerUserGetStat Param Invalid !\n");
        return ;
    }	
	
	pOsa_Entity = (OSA_LogManagerCommon *)malloc(sizeof(OSA_LogManagerCommon)); 
	if (!pOsa_Entity) {
		OSA_ERROR("No Enough mem \n");
		return;
	}
	
	memset(pOsa_Entity, 0, sizeof(OSA_LogManagerCommon));
	
	memcpy(pOsa_Entity->osa_Name, p1, strlen(p1));
	
	ret = OSA_LogManagerGetEntryInfo(pOsa_Entity);
    if (OSA_SOK != ret) {
        printf("OSA_LogManagerGetEntryInfo Get %s Levle Failed!\n", p1);
        return ;
    }

	OSA_INFO("%s osa_PrintEnable %d \n", p1, pOsa_Entity->osa_Stat);
	
	free(pOsa_Entity);
    return ;
}

void OSA_LogManagerUserSetStat(void *p1, void *p2)
{
	int ret;
	int oldStat;
	
	OSA_LogManagerCommon *pOsa_Entity = NULL;
	
	if (!p1 || !p2) {
        printf("OSA_LogManagerUserSetStat Param Invalid !\n");
        return ;
    }	
	
	pOsa_Entity = (OSA_LogManagerCommon *)malloc(sizeof(OSA_LogManagerCommon)); 
	if (!pOsa_Entity) {
		OSA_ERROR("No Enough mem \n");
		return;
	}
	
	memset(pOsa_Entity, 0, sizeof(OSA_LogManagerCommon));	
	memcpy(pOsa_Entity->osa_Name, p1, strlen(p1));
	
	ret = OSA_LogManagerGetEntryInfo(pOsa_Entity);
    if (OSA_SOK != ret) {
        printf("OSA_LogManagerGetEntryInfo Get %s Levle Failed!\n", p1);
        return ;
    }
	
	oldStat = pOsa_Entity->osa_Stat;
	
	pOsa_Entity->osa_Stat = atoi(p2);
	
	ret = OSA_LogManagerSetEntryInfo(pOsa_Entity);
    if (OSA_SOK != ret) {
           OSA_ERROR("OSA_LogManagerSetEntryInfo %s %d to %d Failed !\n", p1, oldStat, atoi(p2));
           return ;
    }		
 
	OSA_INFO("OSA_LogManagerSetEntryInfo %s %d to %d!\n", p1, oldStat, atoi(p2));
	
	free(pOsa_Entity);
    return;
}

void OSA_LogManagerUserGetAllinfo(void)
{
	int ret, i;
	int level;	
	size_t len;
	Uint64 printedBytes = 0;
	
	OSA_LogManagerCommon *pOsa_Entity = NULL;
	
	len = OSA_LogManagerGetAllEntryInfoSize();
	if (len < 0) {
		OSA_ERROR("Get_AllEntryInfoSize Failed \n");
		return;
	}	
	
	pOsa_Entity = (OSA_LogManagerCommon *)malloc(len); 
	if (!pOsa_Entity) {
		OSA_ERROR("No Enough mem \n");
		return;
	}
	
	memset(pOsa_Entity, 0, len);
	
	ret = OSA_LogManagerGetAllEntryInfo(pOsa_Entity, len);
	if (ret != 0) {
		OSA_ERROR("Get_AllEntryInfo Failed \n");
		return;
	}	

	for (i = 0; i < len / sizeof(OSA_LogManagerCommon); i++) {
		printf("Entity %s level %d enable %d stat %d ", pOsa_Entity[i].osa_Name, 
			pOsa_Entity[i].osa_LogLevel, pOsa_Entity[i].osa_PrintEnable, pOsa_Entity[i].osa_Stat);
			
		OSA_LogManagerGetEntryPrintStatics(pOsa_Entity[i].osa_Name, &printedBytes);
		printf("printedBytes %llu \n", printedBytes);
	}
	
	free(pOsa_Entity);
	pOsa_Entity = NULL;
	
	return;
}

void OSA_LogManagerUserRegister(void *p1, void *p2)
{
	int ret;
	int i, j;
	int nr_entry;
	Char osaName[OSA_LOGENTITY_NAMELEN] = {0};
	OSA_LogManager *osa_test;
	
	if (!p1 || !p2) {
        printf("OSA_LogManagerUserRegister Param Invalid !\n");
        return;
    }
	
	nr_entry = atoi(p2);
	osa_test = (OSA_LogManager *)malloc(sizeof(OSA_LogManager) * nr_entry);
	if (!osa_test) {
		OSA_ERROR("No Enough mem \n");
		return;
	}

	memset(osa_test, 0, nr_entry * sizeof(OSA_LogManager));
	
	for (i = 0; i < nr_entry; i++) {
		memset(osaName, 0, OSA_LOGENTITY_NAMELEN);
		sprintf(osaName, "Register_test_%d\n", i);
		
		printf("%s Register... \n", osaName);
		
		osa_test->info.osa_LogLevel = OSA_LOG_LV_DBG;
		
		memcpy(osa_test[i].info.osa_Name, osaName, OSA_LOGENTITY_NAMELEN);
		ret = OSA_logManagerRegister(&osa_test[i]);
		if (ret != OSA_SOK) {
			OSA_ERROR("%s Register Failed... \n", osaName);
			break;
		}		
	}

	for (j = 0; j < i; j++) {	
		ret = OSA_logManagerDeRegister(&osa_test[j]);
		if (ret != OSA_SOK) {
			OSA_ERROR("%s DeRegister Failed... \n", osa_test[j].info.osa_Name);
			break;
		}	
	}
	
	return;
}
static HandleFuncId handle[] =
{
	Handlerfunc(OSA_LogManagerUserGetLevel, GLevel),   
	Handlerfunc(OSA_LogManagerUserSetLevel, SLevel),
	Handlerfunc(OSA_LogManagerUserGetEnable, GEnable),
	Handlerfunc(OSA_LogManagerUserSetEnable, SEnable),
	Handlerfunc(OSA_LogManagerUserGetStat, GStat),
	Handlerfunc(OSA_LogManagerUserSetStat, SStat),
	Handlerfunc(OSA_LogManagerUserGetAllinfo, GetAll),

	Handlerfunc(OSA_LogManagerUserRoundTest, RoundTest),
	Handlerfunc(OSA_LogManagerUserRegister, Register),
	{ NULL,NULL}
};
static int cmp_cmd(char *p, HandleFuncId *cmd_p)
{
   	int i;
  	char *s;
	   	
   	i=0;
	while(cmd_p[i].handlerstr)
	{
   		s = (char *)cmd_p[i].handlerstr;
		if(!strcmp(p, s)) 
		{
            return i;
		}
		
      	i ++;
   	}

   	return -1;
}


void PrintHelp(void)
{
    printf("***********************************\n");
    printf("usage:\n");
	printf("./osaLogCtl GLevel modName \n");              //获取模块/库的打印等级
    printf("./osaLogCtl SLevel modName newLevel\n");      //设置模块/库的打印等级
    printf("./osaLogCtl GEnable modName \n");             //获取模块/库的使能状态
    printf("./osaLogCtl SEnable modName newEnable \n");   //设置模块/库的使能状态
    printf("./osaLogCtl GStat modName \n");               //获取模块/库的统计功能
    printf("./osaLogCtl SStat modName newStat \n");       //设置模块/库的统计功能
    printf("./osaLogCtl GetAll \n");                      //获取所有管理模块信息
    printf("./osaLogCtl RoundTest rounds modName");       //随机测试 rounds表示测试多少轮 modName哪个模块进行特殊处理
    printf("./osaLogCtl Register round modName \n");      //注册和注销处理，参数同上

    printf("***********************************\n");	
}

int main(int32_t argc, char **argv)
{
    int32_t j;
	int ret;

	OSA_LogManager *osa_test;

	if(argc < 2) {
        PrintHelp();
        return 0;
    }
	
	OSA_LogManagerUserInit();
	OSA_INFOR("osaLogCtl test \n");
	
	osa_test = (OSA_LogManager *)malloc(sizeof(OSA_LogManager));
	if (!osa_test) {
		OSA_ERROR("No Enough mem \n");
		OSA_LogManagerUserExit();
		return -1;
	}

	memset(osa_test, 0, sizeof(OSA_LogManager));
	osa_test->info.osa_LogLevel = OSA_LOG_LV_DBG;
	osa_test->info.osa_PrintEnable = OSA_LOGMANAGER_ENABLE;
	
	memcpy(osa_test->info.osa_Name, "OSA_TESTU", strlen("OSA_TESTU"));

	OSA_logManagerRegister(osa_test);	
    
    j = cmp_cmd(argv[1],handle);
    if (j >= 0) {
		FuncHandler funchandler = handle[j].handler;
		if (funchandler) {
            if(argc == 2) {
                funchandler(NULL,NULL);
            } else if(argc == 3) {
                funchandler(argv[2], NULL);

            } else if(argc == 4) {
                funchandler(argv[2], argv[3]);
            }
		}
    } else {
        PrintHelp();
    }

    OSA_logManagerDeRegister(osa_test);	
	OSA_INFO("OSA_logManagerDeRegister Finish \n");
	
	OSA_LogManagerUserExit();	
	OSA_INFO("OSA_LogManagerUser_exit Finish \n");

	free(osa_test);
	
    return 0;
}