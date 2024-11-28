@echo off
REM Step 1: Prompt user for folder name and create folder
set /p folder_name="Please enter a name for the output folder: "
mkdir "%folder_name%"

REM Step 2: Compile the C program
echo Compiling Flow_Free_Generator.c...
gcc Flow_Free_Generator.c -o Flow_Free_Generator.exe
if errorlevel 1 (
    echo Compilation failed. Please check your C code.
    pause
    exit /b
)

REM Step 3: Run the compiled C program to generate .txt files in the specified folder
echo Running Flow_Free_Generator.exe to generate .txt files...
Flow_Free_Generator.exe "%folder_name%"

REM Step 4: Convert .txt files to images using Python script color_auto_process.py
echo Converting .txt files to images...

REM Set the Python interpreter path if necessary
SET PYTHON_PATH="PATH_TO_PYTHON_EXE"
REM 可更改為Python的絕對路徑，例如 "C:\Python39\python.exe"

REM Loop through all .txt files in the specified directory, excluding *_info.txt files
for %%f in ("%folder_name%\*.txt") do (
    echo Processing %%f
    %PYTHON_PATH% color_auto_process.py %%f "%folder_name%"
)

echo Process completed! All output files are saved in folder: %folder_name%
pause
