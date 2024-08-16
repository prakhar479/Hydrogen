#!/usr/bin/env bash

# Save this script as compile_gas.sh and make it executable with:
# chmod +x compile_gas.sh

set -e

# Default values
ASSEMBLER="as"
LINKER="ld"
DEBUG=false
OPTIMIZE=false
KEEP_OBJECT=false
OUTPUT_NAME=""

# Function to display usage information
usage() {
    echo "Usage: $0 [options] <input_file.s>"
    echo "Options:"
    echo "  -o, --output <name>   Specify the output filename (default: input filename without extension)"
    echo "  -d, --debug           Include debugging information"
    echo "  -O, --optimize        Enable optimizations"
    echo "  -k, --keep-object     Keep the intermediate object file"
    echo "  -h, --help            Display this help message"
    exit 1
}

# Parse command-line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -o|--output)
            OUTPUT_NAME="$2"
            shift 2
            ;;
        -d|--debug)
            DEBUG=true
            shift
            ;;
        -O|--optimize)
            OPTIMIZE=true
            shift
            ;;
        -k|--keep-object)
            KEEP_OBJECT=true
            shift
            ;;
        -h|--help)
            usage
            ;;
        *)
            INPUT_FILE="$1"
            shift
            ;;
    esac
done

# Check if input file is provided
if [ -z "$INPUT_FILE" ]; then
    echo "Error: No input file specified."
    usage
fi

# Check if input file exists
if [ ! -f "$INPUT_FILE" ]; then
    echo "Error: Input file '$INPUT_FILE' does not exist."
    exit 1
fi

# Set output name if not specified
if [ -z "$OUTPUT_NAME" ]; then
    OUTPUT_NAME="${INPUT_FILE%.*}"
fi

# Set object file name
OBJECT_FILE="${OUTPUT_NAME}.o"

# Build GAS command
AS_CMD="$ASSEMBLER"
if [ "$DEBUG" = true ]; then
    AS_CMD="$AS_CMD -g"
fi
if [ "$OPTIMIZE" = true ]; then
    AS_CMD="$AS_CMD -O2"
fi
AS_CMD="$AS_CMD -o $OBJECT_FILE $INPUT_FILE"

# Build linker command
LD_CMD="$LINKER -o $OUTPUT_NAME $OBJECT_FILE"

# Execute GAS
echo "Assembling with GAS..."
if ! eval $AS_CMD; then
    echo "Error: Assembly failed."
    exit 1
fi

# Execute linker
echo "Linking..."
if ! eval $LD_CMD; then
    echo "Error: Linking failed."
    exit 1
fi

# Clean up object file if not keeping it
if [ "$KEEP_OBJECT" = false ]; then
    rm -f "$OBJECT_FILE"
fi

echo "Compilation complete. Executable created: $OUTPUT_NAME"

# Make the output file executable
chmod +x "$OUTPUT_NAME"

echo "You can run your program with: ./$OUTPUT_NAME"