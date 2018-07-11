int VMBackDoor(unsigned long *reg_a, unsigned long *reg_b,
      unsigned long *reg_c, unsigned long *reg_d)
{
  unsigned long a, b, c, d;
  b=reg_b?*reg_b:0; c=reg_c?*reg_c:0;
  __try {
    __asm {
      push eax
      push ebx
      push ecx
      push edx

      mov eax, VMWARE_MAGIC
      mov ebx, b
      mov ecx, c
      mov edx, VMWARE_PORT

      in eax, dx

      mov a, eax
      mov b, ebx
      mov c, ecx
      mov d, edx

      pop edx
      pop ecx
      pop ebx
      pop eax
    }
  } __except (EXCEPTION_EXECUTE_HANDLER) {}
  if(reg_a) *reg_a=a;
  if(reg_b) *reg_b=b;
  if(reg_c) *reg_c=c;
  if(reg_d) *reg_d=d;
  return a;
}

int VMGetVersion() {
  unsigned long version, magic, command;
  command=VMCMD_GET_VERSION;
  VMBackDoor(&version, &magic, &command, NULL);
  if(magic==VMWARE_MAGIC) return version;
  else return 0; }

int IsVMWare() {
  int version=VMGetVersion();
  if(version)
    return true;
  else
    return false;
}
