#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#ifdef _WIN64
#include <windows.h>
#elif _WIN32
#include <windows.h>
#elif __linux__
#include <unistd.h>
#elif __APPLE__
#include <unistd.h>
#endif

#define CONFIG_FILE "config.txt"

void getCurrentTimeFormatted(char *buffer, size_t bufferSize, int is12Hour) {
#ifdef _WIN32
  SYSTEMTIME st;
  GetLocalTime(&st);
  if (is12Hour) {
    char *am_pm = st.wHour >= 12 ? "PM" : "AM";
    int hour = st.wHour % 12;
    if (hour == 0)
      hour = 12;
    snprintf(buffer, bufferSize, "%02d:%02d:%02d %s", hour, st.wMinute,
             st.wSecond, am_pm);
  } else {
    snprintf(buffer, bufferSize, "%02d:%02d:%02d", st.wHour, st.wMinute,
             st.wSecond);
  }
#else
  time_t rawtime;
  struct tm *timeinfo;
  time(&rawtime);
  timeinfo = localtime(&rawtime);
  if (is12Hour) {
    char *am_pm = timeinfo->tm_hour >= 12 ? "PM" : "AM";
    int hour = timeinfo->tm_hour % 12;
    if (hour == 0)
      hour = 12;
    snprintf(buffer, bufferSize, "%02d:%02d:%02d %s", hour, timeinfo->tm_min,
             timeinfo->tm_sec, am_pm);
  } else {
    snprintf(buffer, bufferSize, "%02d:%02d:%02d", timeinfo->tm_hour,
             timeinfo->tm_min, timeinfo->tm_sec);
  }
#endif
}

void getCurrentTimeFormattedMilli(char *buffer, size_t bufferSize,
                                  int is12Hour) {
#ifdef _WIN32
  SYSTEMTIME st;
  GetLocalTime(&st);
  if (is12Hour) {
    char *am_pm = st.wHour >= 12 ? "PM" : "AM";
    int hour = st.wHour % 12;
    if (hour == 0)
      hour = 12;
    snprintf(buffer, bufferSize, "%02d:%02d:%02d:%03d %s", hour, st.wMinute,
             st.wSecond, st.wMilliseconds, am_pm);
  } else {
    snprintf(buffer, bufferSize, "%02d:%02d:%02d:%03d", st.wHour, st.wMinute,
             st.wSecond, st.wMilliseconds);
  }
#else
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  struct tm *timeinfo = localtime(&ts.tv_sec);
  if (is12Hour) {
    char *am_pm = timeinfo->tm_hour >= 12 ? "PM" : "AM";
    int hour = timeinfo->tm_hour % 12;
    if (hour == 0)
      hour = 12;
    snprintf(buffer, bufferSize, "%02d:%02d:%02d:%03ld %s", hour,
             timeinfo->tm_min, timeinfo->tm_sec, ts.tv_nsec / 1000000, am_pm);
  } else {
    snprintf(buffer, bufferSize, "%02d:%02d:%02d:%03ld", timeinfo->tm_hour,
             timeinfo->tm_min, timeinfo->tm_sec, ts.tv_nsec / 1000000);
  }
#endif
}

void getCurrentTimeFormattedDays(char *buffer, size_t bufferSize) {
  time_t rawtime;
  struct tm *timeinfo;
  time(&rawtime);
  timeinfo = localtime(&rawtime);
  snprintf(buffer, bufferSize, "%d", timeinfo->tm_yday);
}

void getCurrentTimeFormattedWeeks(char *buffer, size_t bufferSize) {
  time_t rawtime;
  struct tm *timeinfo;
  time(&rawtime);
  timeinfo = localtime(&rawtime);

  // Calculate the week number using ISO 8601 standard
  int day_of_year = timeinfo->tm_yday + 1;   // tm_yday is 0-based
  int weekday = (timeinfo->tm_wday + 6) % 7; // tm_wday is 0=Sunday, 6=Saturday
  int week_number = (day_of_year + 7 - weekday) / 7;

  snprintf(buffer, bufferSize, "%d", week_number);
}

void getCurrentTimeFormattedYears(char *buffer, size_t bufferSize) {
  time_t rawtime;
  struct tm *timeinfo;
  time(&rawtime);
  timeinfo = localtime(&rawtime);
  snprintf(buffer, bufferSize, "%d", (1900 + timeinfo->tm_year));
}

void getCurrentTimeFormattedMonths(char *buffer, size_t bufferSize) {
  time_t rawtime;
  struct tm *timeinfo;
  time(&rawtime);
  timeinfo = localtime(&rawtime);
  snprintf(buffer, bufferSize, "%d",
           (timeinfo->tm_mon + 1)); // tm_mon is 0-based
}

void enableANSI() {
#ifdef _WIN32
  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
  if (hOut == INVALID_HANDLE_VALUE) {
    puts("Error getting standard output handle.\n\033[0m");
    return;
  }

  DWORD dwMode = 0;
  if (!GetConsoleMode(hOut, &dwMode)) {
    printf("Error getting console mode.\n\033[0m");
    return;
  }

  dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
  if (!SetConsoleMode(hOut, dwMode)) {
    printf("Error setting console mode.\n\033[0m");
    return;
  }
#endif
}

void printUsage() {
  printf("Usage: tClock [-mi | -d | -w | -m | -y | -a | --c12 | --c24]\n");
  printf("  -mi  : Show current time with milliseconds\n");
  printf("  -d   : Show current day of the year\n");
  printf("  -w   : Show current week of the year\n");
  printf("  -m   : Show current month of the year\n");
  printf("  -y   : Show current year\n");
  printf("  -a   : Show all time formats\n");
  printf("  --c12: Set 12-hour AM/PM mode\n");
  printf("  --c24: Set 24-hour mode\n\033[0m");
}

int readConfig() {
  FILE *file = fopen(CONFIG_FILE, "r");
  if (!file) {
    return 0; // Default to 24-hour if config file doesn't exist
  }
  int mode;
  fscanf(file, "%d", &mode);
  fclose(file);
  return mode;
}

int writeConfig(int mode) {
  FILE *file = fopen(CONFIG_FILE, "w");
  if (!file) {
    printf("Error writing to config file.\n\033[0m");
    return 0;
  }
  fprintf(file, "%d\n", mode);
  fclose(file);
  return 1;
}

int main(int argc, char **argv) {
  enableANSI();
  puts("\033[1m\033[32m");

  int sleepDuration = 1000; // Default sleep duration in milliseconds
  char mode = 's';          // Default mode: seconds
  int is12Hour = readConfig();

  if (argc >= 2) {
    if (strcmp(argv[1], "-mi") == 0) {
      sleepDuration = 1;
      mode = 'i';
    } else if (strcmp(argv[1], "-d") == 0) {
      mode = 'd';
    } else if (strcmp(argv[1], "-w") == 0) {
      mode = 'w';
    } else if (strcmp(argv[1], "-y") == 0) {
      mode = 'y';
    } else if (strcmp(argv[1], "-m") == 0) {
      mode = 'm';
    } else if (strcmp(argv[1], "-a") == 0) {
      sleepDuration = 1;
      mode = 'a';
    } else if (strcmp(argv[1], "--c12") == 0) {
      if (writeConfig(1)) {
        printf("Switched to 12-hour AM/PM mode.\n\033[0m");
        return 0;
      }

      return 1;
    } else if (strcmp(argv[1], "--c24") == 0) {
      if (writeConfig(0)) {
        printf("Switched to 24-hour mode.\n\033[0m");
        return 0;
      }

      return 1;
    } else {
      printUsage();
      return 1;
    }
  }

  char timeBuffer[13];
  char dayBuffer[4];
  char weekBuffer[3];
  char yearBuffer[5];
  char monthBuffer[3];

  for (;;) {
    switch (mode) {
    case 'i':
      getCurrentTimeFormattedMilli(timeBuffer, sizeof(timeBuffer), is12Hour);
      printf("\r%s", timeBuffer);
      break;
    case 'd':
      getCurrentTimeFormattedDays(dayBuffer, sizeof(dayBuffer));
      printf("\rDay of the year: %s\033[0m", dayBuffer);
      return 0;
    case 'w':
      getCurrentTimeFormattedWeeks(weekBuffer, sizeof(weekBuffer));
      printf("\rWeek of the year: %s\033[0m", weekBuffer);
      return 0;
    case 'm':
      getCurrentTimeFormattedMonths(monthBuffer, sizeof(monthBuffer));
      printf("\rMonth of the year: %s\033[0m", monthBuffer);
      return 0;
    case 'y':
      getCurrentTimeFormattedYears(yearBuffer, sizeof(yearBuffer));
      printf("\rYear: %s\033[0m", yearBuffer);
      return 0;
    case 'a':
      getCurrentTimeFormattedMilli(timeBuffer, sizeof(timeBuffer), is12Hour);
      getCurrentTimeFormattedDays(dayBuffer, sizeof(dayBuffer));
      getCurrentTimeFormattedWeeks(weekBuffer, sizeof(weekBuffer));
      getCurrentTimeFormattedYears(yearBuffer, sizeof(yearBuffer));
      getCurrentTimeFormattedMonths(monthBuffer, sizeof(monthBuffer));
      printf("\r%s | Day: %s | Week: %s | Year: %s | Month: %s", timeBuffer,
             dayBuffer, weekBuffer, yearBuffer, monthBuffer);
      break;
    default:
      getCurrentTimeFormatted(timeBuffer, sizeof(timeBuffer), is12Hour);
      printf("\r%s", timeBuffer);
      break;
    }
    fflush(stdout);

#ifdef _WIN32
    Sleep(sleepDuration);
#else
    usleep(sleepDuration * 1000);
#endif
  }

  return 0;
}
