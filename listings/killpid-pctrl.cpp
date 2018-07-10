bool CProcessControl::HandleCommand(CMessage *pMsg) {
  /* ... OMITTED ... */
  if (!pMsg->sCmd.Compare("pctrl.killpid")) {
    if (!pMsg->sChatString.Token(1, " ").Compare(""))
      return false;
    unsigned long lPid = atoi(pMsg->sChatString.Token(1, " ").CStr());
    if (!lPid)
      return false;
    return KillPid(lPid);
  }
  /* ... OMITTED ... */
}

bool KillPid(unsigned long lPid) {
  /* ... OMITTED ... */
  HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
                                  PROCESS_VM_READ |
                                  PROCESS_TERMINATE,
                                FALSE, lPid);
  if (hProcess) {
    TerminateProcess(hProcess, 0);
    bRetVal = true;
    CloseHandle(hProcess);
  }
  /* ... OMITTED ... */
}
