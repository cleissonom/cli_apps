# Raw HTTP Request Tool

## Overview

This C program leverages the power of libcurl to send raw HTTP requests configured from a `.txt` file. It supports GET and POST methods and saves the complete response, including headers and body, to a specified output file.

## Features

- **Raw HTTP Request Configuration**: Configure HTTP requests in a `.txt` file, specifying the method, path, HTTP version, host, headers, and body.
- **Complete Response Capture**: Captures and saves the complete HTTP response, including headers and body, to an output file.
- **Host Validation**: Automatically adds `http://` to the host if it is not present.

## Prerequisites

Before running this program, ensure that you have the following installed on your system:

- GCC (GNU Compiler Collection)
- libcurl (C library for transferring data with URLs)

## Compile

```bash
gcc -o raw_http raw_http.c -lcurl
```

## Usage

To use the program, run it from the command line by providing the input file with the raw HTTP request and the output file to save the response:

```bash
./raw_http <input_file.txt> <output_file.txt>
```

## Example

Create a file named request_get.txt with the following content to perform a GET request:

```txt
GET / HTTP/1.1
Host: example.com
User-Agent: raw_http/1.0
Accept: text/html
Connection: close
```

Run the tool:

```bash
./raw_http request_get.txt response_get.txt
```

This command sends the GET request specified in request_get.txt and saves the complete response to response_get.txt.

## Acknowledgements

[libcurl](https://curl.se/libcurl/) for providing a powerful library.
