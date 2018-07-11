struct items {
  char  *GameName;
  HKEY  RootKey;
  char  *SubKey;
  char  *ValueName;};

items CDKeyList[] = {
  {"[Windows Product ID: ", HKEY_LOCAL_MACHINE,
   "Software\\Microsoft\\Windows\\CurrentVersion", "ProductID"},
  {"[Battlefield 1942: ", HKEY_LOCAL_MACHINE,
   "SOFTWARE\\Electronic Arts\\EA GAMES\\Battlefield 1942\\ergc", ""},
  {"[Battlefield 1942: Secret Weapons Of WWII: ", HKEY_LOCAL_MACHINE,
   "SOFTWARE\\Electronic Arts\\EA GAMES\\Battlefield 1942 Secret Weapons of WWII\\ergc", ""},
  {"[Battlefield 1942: The Road To Rome: ", HKEY_LOCAL_MACHINE,
   "SOFTWARE\\Electronic Arts\\EA GAMES\\Battlefield 1942 The Road to Rome\\ergc", ""},
  {"[Battlefield 1942: Vietnam:", HKEY_LOCAL_MACHINE,
   "SOFTWARE\\Electronic Arts\\EA GAMES\\Battlefield Vietnam\\ergc", ""},
  {"[Black and White: ", HKEY_LOCAL_MACHINE,
   "SOFTWARE\\Electronic Arts\\EA GAMES\\Black and White\\ergc", ""},
  {"[Command and Conquer: Generals: ", HKEY_LOCAL_MACHINE,
   "SOFTWARE\\Electronic Arts\\EA GAMES\\Generals\\ergc",  ""},
  {"[Command and Conquer: Generals: Zero Hour: ", HKEY_LOCAL_MACHINE,
   "SOFTWARE\\Electronic Arts\\EA GAMES\\Command and Conquer Generals Zero Hour\\ergc", ""},
  /* ... OMMITED ... */
};
