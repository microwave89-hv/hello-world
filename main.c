// Simplified definitions derived from EFI 1.10 Specification, revision 1.0
typedef struct _EFI_TABLE_HEADER {
	unsigned long long Signature;
	unsigned long Revision;
	unsigned long HeaderSize;
	unsigned long CRC32;
	unsigned long Reserved;
} EFI_TABLE_HEADER;

typedef struct _SIMPLE_TEXT_OUTPUT_INTERFACE SIMPLE_TEXT_OUTPUT_INTERFACE;
typedef long (*FPEFI_TEXT_STRING)(SIMPLE_TEXT_OUTPUT_INTERFACE*, unsigned short*);

typedef struct _SIMPLE_TEXT_OUTPUT_INTERFACE {
	void* pUnused0;
	FPEFI_TEXT_STRING fpOutputString;
} SIMPLE_TEXT_OUTPUT_INTERFACE;

typedef struct {
  EFI_TABLE_HEADER unused0;
  short* pUnused1;
  unsigned long unused2;
  void* pUnused3;
  void* pUnused4;
  void* pConsoleOutHandle;
  SIMPLE_TEXT_OUTPUT_INTERFACE* pConOut;
} EFI_SYSTEM_TABLE;
// end type definitions

#define MAX_ADDRESS_SIZE sizeof(long long)         // 64-bits addresses
#define MAX_PRINT_CHARS (MAX_ADDRESS_SIZE * 2 + 1) // Bytes to nibbles + null terminator

long print_hex_value(unsigned long long raw_value, SIMPLE_TEXT_OUTPUT_INTERFACE* con_out)
{
    unsigned char raw2hex_table[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
    unsigned char hex_value[MAX_PRINT_CHARS];
    unsigned short hex_value_char16[MAX_PRINT_CHARS];
    for (unsigned int i = 0; i < sizeof(hex_value)/sizeof(*hex_value); ++i)
    {
	hex_value[i] = 0;
	hex_value_char16[i] = 0;
    }
	
    for (int i = MAX_ADDRESS_SIZE * 2 - 1; i >= 0; --i)
    {
        unsigned char nibble_value = raw_value % (1 << 4);
	unsigned char hex_char = raw2hex_table[nibble_value]; // Figure which char to print
	hex_value[i] = hex_char;
	raw_value >>= 4;
    }
	
    for (unsigned int i = 0; i < sizeof(hex_value)/sizeof(*hex_value); ++i)
    {
	hex_value_char16[i] = (unsigned short)hex_value[i]; // SIMPLE_TEXT_OUTPUT.OutputString() expects double char but hex_value_char16 was calculated in char
    }

    con_out->fpOutputString(con_out, L"\r\n");
    con_out->fpOutputString(con_out, hex_value_char16);
    return 0;
}

long efi_miau(void* pUnused0, EFI_SYSTEM_TABLE* pEfiSystemTable)
{
    unsigned long long ptr_value = (unsigned long long)pEfiSystemTable;

    SIMPLE_TEXT_OUTPUT_INTERFACE* con_out = pEfiSystemTable->pConOut;
    print_hex_value(ptr_value, con_out);                                // Print address of the EFI_SYSTEM_TABLE
    print_hex_value(0xffff800000000000 + ptr_value, con_out); 		// Verify that it works with 64 bits too
    print_hex_value(0xffff900000000000 + ptr_value + 3, con_out); 	// Verify that it works with non-aligned values too
    return 0;
}
