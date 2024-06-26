struct sysinfo {
  time_t uptime;
  float memFree;
  float memUsage;
  float memTot;
  float memPercent;
  int temp;
  float cpu;
} sysinfo;
struct board {
  const char* kernel;
  const char* hostName;
  const char* version;
} board;
struct vnstat {
  float todayUsageTX;
  float todayUsageRX;
  float todayTotal;
  float yesterdayUsageTX;
  float yesterdayUsageRX;
  float monthUsageTX;
  float monthUsageRX;
  float monthTotal;
  float lastMonthUsageTX;
  float lastMonthUsageRX;
  float totalUsageTX;
  float totalUsageRX;
} vnstat;
struct netdata {
  float UPLOAD;
  float DOWNLOAD;
} netdata;
