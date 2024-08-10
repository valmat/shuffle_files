# Shuffle Files

The `shuffle_files` project is a command-line tool designed to shuffle, copy, or move files from one directory to another. This tool reads all files from a given input directory (including subdirectories), optionally filters them by a specified file extension, randomly shuffles them, and then copies or moves them to an output directory. The files are renamed in a sequential order with leading zeros to maintain a natural lexicographical order.

## Installation

```bash
git clone --recursive https://github.com/valmat/shuffle_files
cd shuffle_files
```
or
```bash
git clone https://github.com/valmat/shuffle_files
cd shuffle_files
git submodule update --init --recursive
```

Next, go to the `src` directory and compile the project:

```bash
cd src
make release
```

If you need a debug build, use the following command:
```bash
cd src
make -j
```

## Main Features
- **Input and Output Directories**: Specify the input directory and optionally the output directory.
- **File Extension Filtering**: Filter files by a specified extension (e.g., `.mp3`).
- **Copy or Move**: Choose to copy (default) or move the files.
- **Verbose Mode**: Optionally enable verbose mode to see detailed output.
- **Sequential Renaming**: Files are renamed with leading zeros to preserve order.


## Usage

Here's how to use this utility:

```bash
$ bin/shuffle_files.bin -h

bin/shuffle_files.bin [options]
    -h, --help
        Shows this help message
    -i, --in, --dir
        Input directory
    -o, --out
        Output directory (optional, defaults to input directory)
    --cp
        Copy files (default behavior)
    --mv
        Move files instead of copying
    -v, --verb
        Verbose mode
    -e, --ext
        File extension to filter (e.g. --ext ".mp3")
```

For example:

```sh
shuffle_files -i /path/to/input/dir -o /path/to/output/dir --ext ".mp3" --mv -v
```

## Examples
1. Shuffle and copy all files from the input directory to the output directory:
   ```sh
   shuffle_files -i /path/to/input/dir -o /path/to/output/dir
   ```

2. Shuffle and move all `.mp3` files from the input directory to the output directory:
   ```sh
   shuffle_files -i /path/to/input/dir -o /path/to/output/dir --ext ".mp3" --mv
   ```

3. Shuffle and copy all files from the input directory to the input directory with verbose output:
   ```sh
   shuffle_files -i /path/to/input/dir -v
   ```

## Source Code
[https://github.com/valmat/shuffle_files](https://github.com/valmat/shuffle_files)

## License
[The MIT License](LICENSE)
