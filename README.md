# tClock

`tClock` is a command-line tool for displaying the current time and date in either 12h or 24h format.

## Features

- **12-hour AM/PM and 24-hour time formats** 
- **Display time with milliseconds**
- **Show day of the year**
- **Show week of the year**
- **Show month of the year**
- **Show current year**
- **Display all available time formats**

## Installation

You can compile `tClock` using a C compiler. To do this, save the source code into a file named `tClock.c`, and then use the following command to compile it:

```sh
gcc tClock.c -o tClock
```

This will produce an executable named `tClock` which you can then run from the terminal.

## Usage

```sh
tClock [-mi | -d | -w | -m | -y | -a | --c12 | --c24]
```

### Options

- `-mi` : Show current time with milliseconds.
- `-d`  : Show the current day of the year.
- `-w`  : Show the current week of the year.
- `-m`  : Show the current month of the year.
- `-y`  : Show the current year.
- `-a`  : Show all available time formats (time with milliseconds, day, week, month, and year).
- `--c12` : Switch to 12-hour AM/PM mode.
- `--c24` : Switch to 24-hour mode.

### Configuration

The time format mode (12-hour or 24-hour) is saved in a configuration file named `config.txt`. By default, the program uses 24-hour mode if the file does not exist.

- `--c12` sets the mode to 12-hour AM/PM.
- `--c24` sets the mode to 24-hour.

Running `tClock --c12` or `tClock --c24` will update the configuration file and switch the time format accordingly.

### Example

To display the current time with milliseconds in 12-hour AM/PM format:

```sh
./tClock -mi
```

To display all available time formats with the current 12-hour mode:

```sh
./tClock -a
```

To switch to 24-hour mode:

```sh
./tClock --c24
```

To show the current day of the year:

```sh
./tClock -d
```

## Notes

- The program uses ANSI escape codes for formatting text in the terminal. This might not be supported on all terminals.
- The configuration file `config.txt` is used to persist the user's preferred time format mode.

## License

This program is licensed under the MIT License. See the [LICENSE](LICENSE) file for more information.

Feel free to adjust the content to better fit your project's needs or to add additional details specific to your implementation!
