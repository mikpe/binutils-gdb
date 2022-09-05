SCRIPT_NAME=elf
OUTPUT_FORMAT="elf32-pj"
TEXT_START_ADDR=0x1000000
MAXPAGESIZE=0x1000
ARCH=pj
TEMPLATE_NAME=elf32

# code fragments to call back into bfd to create the magic sections
# needed for function call fixup thunks.
CREATE_THUNK_BFD="
  {
    LANG_FOR_EACH_INPUT_STATEMENT (is)
      {
	if (!is->next)
	  bfd_elf32_pj_create_thunk_bfd (is->the_bfd, & link_info);
      }
  }"

FINISH_THUNK_BFD="
  {
    LANG_FOR_EACH_INPUT_STATEMENT (is)
      {
	if (!bfd_elf32_pj_process_thunk_bfd (is->the_bfd, & link_info))
	  {
	    einfo (_(\"Errors encountered processing file %s\"), is->filename);
	  }
     }
  }"
