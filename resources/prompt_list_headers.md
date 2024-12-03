# "List headers" program logic:
1. Write a class TapeScanner
2. It's purpose is to scan tape file for `header_t` by the header `collection_name`. And return the result as list of tuple (`header_offset`, `header_t`), ``
3. This is a prototype so don't overly complicate the code. Keep the code structure clean and well organize.  
4. Write a method similar to `Application.convertEbcdicFileToAscii()`:
- It has 2 parameters: `filename`, `collection_name` 
- it reads file with MmapFileReader
- it encode `collection_name` to ebcdic
- it scan the tape with TapeScanner
- it print as json the result and store it in a file called `{filename}_headers.json`.
  - the json structure:
  ```json
    [
      {
        "header_offset": header_offset,
        "collection_name": collection_name,
        "file_name": file_name,
        "filesize": filesize,
        "unknown": unknown,
        "segsize": segsize,
        "oam_tag": oam_tag
      },
      ...
    ]
  ```