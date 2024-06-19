# Currency Converter Pay Calculator

## Overview

This C program leverages the power of libcurl to fetch real-time exchange rates, specifically between USD and BRL (Brazilian Real). It calculates the total payment due in both USD and BRL for a given time worked, expressed as hours, minutes, and seconds, based on an hourly rate.

## Features

- **Real-time Currency Conversion**: Fetches live exchange rates to ensure accurate conversion between USD and BRL.
- **Custom Time Input**: Allows users to input time in a flexible `HH:MM:SS` format.
- **Dynamic Memory Management**: Utilizes dynamic memory allocation to handle web data fetching and parsing.

## Prerequisites

Before running this program, ensure that you have the following installed on your system:

- GCC (GNU Compiler Collection)
- libcurl (C library for transferring data with URLs)

## Compile

```bash
make build
```

## Usage

To use the program, run it from the command line by providing the hourly rate and the time worked as arguments:

```bash
./salary_calculator <hourly_rate> <hours:minutes:seconds>
```

## Example
```bash
./salary_calculator 25.00 8:30:45
```

This command calculates the payment for 8 hours, 30 minutes, and 45 seconds at a rate of $25.00 per hour, converting the total from USD to BRL using the current exchange rate.

## Acknowledgements

[AwesomeAPI](https://docs.awesomeapi.com.br/api-de-moedas) for providing exchange rate APIs.
