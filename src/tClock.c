#include <stdio.h>
#include <string.h>
#include <time.h>

#ifdef _WIN64
#include <windows.h>
#elif _WIN32
#include <windows.h>
#elif __linux__
#include <unistd.h>
#elif __APPLE__
#include <unistd.h>
#endif

void GetCurrentTimeFormatted(char *buffer, size_t bufferSize) {
#ifdef _WIN32
  SYSTEMTIME st;
  GetLocalTime(&st);
  snprintf(buffer, bufferSize, "%02d:%02d:%02d", st.wHour, st.wMinute,
           st.wSecond);
#else
  time_t rawtime;
  struct tm *timeinfo;
  time(&rawtime);
  timeinfo = localtime(&rawtime);
  snprintf(buffer, bufferSize, "%02d:%02d:%02d", timeinfo->tm_hour,
           timeinfo->tm_min, timeinfo->tm_sec);
#endif
}

void GetCurrentTimeFormattedMilli(char *buffer, size_t bufferSize) {
#ifdef _WIN32
  SYSTEMTIME st;
  GetLocalTime(&st);
  snprintf(buffer, bufferSize, "%02d:%02d:%02d:%03d", st.wHour, st.wMinute,
           st.wSecond, st.wMilliseconds);
#else
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  struct tm *timeinfo = localtime(&ts.tv_sec);
  snprintf(buffer, bufferSize, "%02d:%02d:%02d:%03ld", timeinfo->tm_hour,
           timeinfo->tm_min, timeinfo->tm_sec, ts.tv_nsec / 1000000);
#endif
}

void GetCurrentTimeFormattedDays(char *buffer, size_t bufferSize) {
  time_t rawtime;
  struct tm *timeinfo;
  time(&rawtime);
  timeinfo = localtime(&rawtime);
  snprintf(buffer, bufferSize, "%d", timeinfo->tm_yday);
}

void GetCurrentTimeFormattedWeeks(char *buffer, size_t bufferSize) {
  time_t rawtime;
  struct tm *timeinfo;
  time(&rawtime);
  timeinfo = localtime(&rawtime);
  snprintf(buffer, bufferSize, "%d", (timeinfo->tm_yday / 7));
}

void GetCurrentTimeFormattedYears(char *buffer, size_t bufferSize) {
  time_t rawtime;
  struct tm *timeinfo;
  time(&rawtime);
  timeinfo = localtime(&rawtime);
  snprintf(buffer, bufferSize, "%d", (1900 + timeinfo->tm_year));
}

void EnableANSI() {
#ifdef _WIN32
  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
  if (hOut == INVALID_HANDLE_VALUE) {
    puts("Error getting standard output handle.");
    return;
  }

  DWORD dwMode = 0;
  if (!GetConsoleMode(hOut, &dwMode)) {
    printf("Error getting console mode.");
    return;
  }

  dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
  if (!SetConsoleMode(hOut, dwMode)) {
    printf("Error setting console mode.");
    return;
  }
#endif
}

void PrintUsage() {
  printf("Usage: timeapp [-m | -d | -w | -y | -a]\n");
  printf("  -m : Show current time with milliseconds\n");
  printf("  -d : Show current day of the year\n");
  printf("  -w : Show current week of the year\n");
  printf("  -y : Show current year\n");
  printf("  -a : Show all time formats\n");
}

int main(int argc, char **argv) {
  EnableANSI();
  puts("\033[1m\033[32m");

  int sleepDuration = 1000; // Default sleep duration in milliseconds
  char mode = 's';          // Default mode: seconds

  if (argc >= 2) {
    if (strcmp(argv[1], "-m") == 0) {
      sleepDuration = 1;
      mode = 'm';
    } else if (strcmp(argv[1], "-d") == 0) {
      mode = 'd';
    } else if (strcmp(argv[1], "-w") == 0) {
      mode = 'w';
    } else if (strcmp(argv[1], "-y") == 0) {
      mode = 'y';
    } else if (strcmp(argv[1], "-a") == 0) {
      sleepDuration = 1;
      mode = 'a';
    } else {
      PrintUsage();
      return 1;
    }
  }

  char timeBuffer[13];
  char dayBuffer[4];
  char weekBuffer[3];
  char yearBuffer[5];

  for (;;) {
#ifdef _WIN32
    Sleep(sleepDuration);
#else
    usleep(sleepDuration * 1000);
#endif

    switch (mode) {
    case 'm':
      GetCurrentTimeFormattedMilli(timeBuffer, sizeof(timeBuffer));
      printf("\r%s", timeBuffer);
      break;
    case 'd':
      GetCurrentTimeFormattedDays(dayBuffer, sizeof(dayBuffer));
      printf("\rDay of the year: %s\033[0m", dayBuffer);
      return 0;
    case 'w':
      GetCurrentTimeFormattedWeeks(weekBuffer, sizeof(weekBuffer));
      printf("\rWeek of the year: %s\033[0m", weekBuffer);
      return 0;
    case 'y':
      GetCurrentTimeFormattedYears(yearBuffer, sizeof(yearBuffer));
      printf("\rYear: %s\033[0m", yearBuffer);
      return 0;
    case 'a':
      GetCurrentTimeFormattedMilli(timeBuffer, sizeof(timeBuffer));
      GetCurrentTimeFormattedDays(dayBuffer, sizeof(dayBuffer));
      GetCurrentTimeFormattedWeeks(weekBuffer, sizeof(weekBuffer));
      GetCurrentTimeFormattedYears(yearBuffer, sizeof(yearBuffer));
      printf("\r%s | Day: %s | Week: %s | Year: %s", timeBuffer, dayBuffer,
             weekBuffer, yearBuffer);
      break;
    default:
      GetCurrentTimeFormatted(timeBuffer, sizeof(timeBuffer));
      printf("\r%s", timeBuffer);
      break;
    }
    fflush(stdout);
  }

  return 0;
}
