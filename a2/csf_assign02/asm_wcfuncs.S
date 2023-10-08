/*
 * Assembly language function implementations
 */

#define MAX_WORDLEN 63
#define WORDENTRY_SIZE (WORDENTRY_NEXT_OFFSET+8)

/*
 * Offsets for the fields of the struct WordEntry data type
 * (Note that there is 4 bytes of padding between the count and next
 * fields.)
 */
#define WORDENTRY_WORD_OFFSET   (0)
#define WORDENTRY_COUNT_OFFSET  (MAX_WORDLEN+1)
#define WORDENTRY_NEXT_OFFSET   (MAX_WORDLEN+1+4+4)

	.section .rodata
/* Define any string constants or read-only data here */

	.section .text

/*
 * Compute a hash code for the given NUL-terminated
 * character string.
 *
 * The hash algorithm should be implemented as follows:
 *
 * uint32_t hash_code = 5381
 * for each character c of w in order {
 *   hash_code = hash_code * 33 + c
 * }
 *
 * Note that the character values should be treated as
 * being unsigned (in the range 0..255)
 *
 * C function prototype:
 *    uint32_t wc_hash(const unsigned char *w);
 */
.globl wc_hash
wc_hash:
    # Initialize hash_code with 5381
    movl $5381, %eax

.Lloop_start:
    # Load next byte (character) of string into ECX
    movzbl (%rdi), %ecx
    # Test if loaded byte is NUL (end of string)
    testl %ecx, %ecx
    jz .Lloop_end

    # Multiply EAX (hash_code) by 33 
    imull $33, %eax, %eax

    # Add the byte (character) value to EAX
    addl %ecx, %eax

    # Move to next character in string
    inc %rdi
    # Repeat loop for next character
    jmp .Lloop_start

.Lloop_end:
    ret


/*
 * Compare two strings lexicographically. Return
 *
 * - a negative value if lhs string is less than rhs string
 * - 0 if lhs string is identical to rhs string
 * - a positive value if lhs string is greater than rhs string
 *
 * Lexicographical comparison is a generalization of alphabetical
 * order, but using character codes. If one string is a prefix
 * of the other, it is considered as "less than". E.g.,
 * "hi" would compare as less than "high".
 *
 * C function prototype:
 *    int wc_str_compare(const unsigned char *lhs, const unsigned char *rhs);
 */
.globl wc_str_compare
wc_str_compare:
    movzbl (%rdi), %eax  # Load the first byte of lhs into 32 bit eax register
    movzbl (%rsi), %ecx  # Load the first byte of rhs into 32 bit ecx register

    # Check if we are at the end of either string
.Ltop:  
    test %eax, %eax      # Is lhs byte '\0'
	# If lhs is null terminated, that means rhs could be greater or could potentially also end there
    jz .Lcheck_rhs_end_of_string

    test %ecx, %ecx      # Is rhs byte '\0'
	# If rhs is null terminated, by this point we are sure that lhs is greater
    jz .Llhs_is_greater

    # Compare characters from lhs and rhs
    cmp %ecx, %eax
	# If different characters jump to Lfound_difference
    jne .Lnot_same

    # Move to the next character
    inc %rdi
    inc %rsi

    # Load next characters for comparison in the next iteration
    movzbl (%rdi), %eax  # Load next byte of lhs
    movzbl (%rsi), %ecx  # Load next byte of rhs
    jmp .Ltop

.Lcheck_rhs_end_of_string:
    test %ecx, %ecx     # Check if rhs byte is '\0'
	# If it is, that means both lhs and rhs ended and they are equal
    jz .Lstrings_equal
	# Else, rhs is indeed greater than lhs so return a negative value
    mov $-1, %eax
    ret

.Llhs_is_greater:
    mov $1, %eax
    ret

.Lstrings_equal:
    mov $0, %eax      # Set return value to 0 as strings are identical
    ret

.Lnot_same:
    sub %ecx, %eax     # Subtract to get the difference
    ret

/*
 * Copy NUL-terminated source string to the destination buffer.
 *
 * C function prototype:
 *    void wc_str_copy(unsigned char *dest, const unsigned char *source);
 */
.globl wc_str_copy
wc_str_copy:
    
.Lbody: # loop body that will continue while *source is not at null terminator/not null
    movzbl (%rsi), %eax # Move first byte of SOURCE argument into 16-bit register
    
    test %al, %al # tests if SOURCE byte is a null terminator/end of string
    jz .Lend # jump to loop end if SOURCE byte is a null terminator/end of string
    
    movb %al, (%rdi) # moving SOURCE byte into DEST byte

    inc %rdi # incrementing char pointer to next char in array
    inc %rsi

    jmp .Lbody
    
.Lend:    
    movb $0x00, (%rdi) # Add null terminator to DEST
    ret


/*
 * Return 1 if the character code in c is a whitespace character,
 * false otherwise.
 *
 * For the purposes of this function, a whitespace character is one of
 *
 *   ' '
 *   '\t'
 *   '\r'
 *   '\n'
 *   '\f'
 *   '\v'
 *
 * C function prototype:
 *    int wc_isspace(unsigned char c);
 */
	.globl wc_isspace
wc_isspace:
	/* TODO: implement */
	cmpb $0x20, %dil # ' '
	je .Lwhitespace

	cmpb $0x09, %dil # '\t'
	je .Lwhitespace

	cmpb $0x0D, %dil # '\r'
	je .Lwhitespace

	cmpb $0x0A, %dil # '\n'
	je .Lwhitespace

	cmpb $0x0C, %dil # '\f'
	je .Lwhitespace

	cmpb $0x0B, %dil # '\v'
	je .Lwhitespace

	mov $0, %eax
	ret

.Lwhitespace:
	mov $1, %eax
	ret

/*
 * Return 1 if the character code in c is an alphabetic character
 * ('A' through 'Z' or 'a' through 'z'), 0 otherwise.
 *
 * C function prototype:
 *    int wc_isalpha(unsigned char c);
 */
	.globl wc_isalpha
wc_isalpha:
    mov %dil, %al

    # Check if c is within 'A' to 'Z'
    subb $0x41, %dil
    cmpb $0x5A - 0x41, %dil
    jbe .Lis_alpha

    # Check if c is within 'a' to 'z'
    subb $0x61, %al
    cmpb $0x7A - 0x61, %al
    jbe .Lis_alpha

    mov $0, %eax
    ret

.Lis_alpha:
    mov $1, %eax
    ret

/*
 * Read the next word from given input stream, storing
 * the word in the array pointed-to by w. (This array should be
 * assumed to be MAX_WORDLEN+1 elements in size.) Return 1
 * if a word is read successfully, 0 otherwise.
 *
 * For the purposes of this function, a word is a sequence of
 * 1 or more non-whitespace characters.
 *
 * If a sequence of non-whitespace characters has more than
 * MAX_WORDLEN characters, then only the first MAX_WORDLEN
 * characters in the sequence should be stored in the array.
 *
 * C function prototype:
 *    int wc_readnext(FILE *in, unsigned char *w);
 */
.globl wc_readnext
wc_readnext:
    # Preserve the callee-saved registers to the stack
    pushq %rbp        # Save old frame pointer
    pushq %r12        # Save %r12
    pushq %r13        # Save %r13
    pushq %r14        # Save %r14
    pushq %r15        # Save %r15

    subq $8, %rsp

    movq %rdi, %r12   # Store file pointer in %r12
    movq %rsi, %r13   # Store pointer to the word buffer in %r13

    movq $0, %r14     # Initialize return value (success flag) to 0
    movq $0, %rbp     # Initialize character counter to 0

    # Check if file pointer is valid
    testq %r12, %r12
    jle .LEndOfReading # If not jump to function exit

    # Start skipping any leading whitespace characters
.LSkipLeadingWhitespaces:
    movq %r12, %rdi
    call fgetc         # Read the next character from the file
    movq %rax, %r15   # Store the read character in %r15

    # Check if the character is EOF or out of ASCII bounds
    testq %r15, %r15
    js .LEndOfReading
    cmpq $127, %r15
    jg .LEndOfReading

    # Determine if the character is a whitespace
    movq %r15, %rdi
    call wc_isspace
    cmpq $0, %rax
    je .LStartReadingWord # If not whitespace, begin reading word
    jmp .LSkipLeadingWhitespaces

# Start reading the word from the file
.LStartReadingWord:
    movq $1, %r14     # Set the success flag since we started reading a word

.LWordLoop:
    movq %r15, (%r13, %rbp) # Store the character in the word buffer
    incq %rbp # Move to the next position in the buffer

    # Check if exceeding maximum word len
    movq $1, %r8
    addq $MAX_WORDLEN, %r8
    cmpq %rbp, %r8
    je .LEndOfReading  # If exceeded, jump to function exit

    # Read the next character
    movq %r12, %rdi
    call fgetc
    movq %rax, %r15

    # Check if character is EOF or out of ASCII bounds
    cmpq $0, %r15
    jle .LEndOfReading
    cmpq $127, %r15
    jg .LEndOfReading

    # Determine if character is a whitespace (signaling end of the word)
    movq %r15, %rdi
    call wc_isspace
    cmpq $1, %rax
    je .LEndOfReading
    jmp .LWordLoop

.LEndOfReading:
    movq $0, (%r13, %rbp) # Null-terminate the word in the buffer
    movq %r14, %rax # Store return value to %rax (success flag)

    # Restore original values of the callee-saved registers from the stack
    addq $8, %rsp
    popq %r15
    popq %r14
    popq %r13
    popq %r12
    popq %rbp
    ret


/*
 * Convert the NUL-terminated character string in the array
 * pointed-to by w so that every letter is lower-case.
 *
 * C function prototype:
 *    void wc_tolower(unsigned char *w);
 */
	.globl wc_tolower
wc_tolower:
	.Lloop_start:
		movzbl (%rdi), %eax
		testb %al, %al
		jz .Lend_of_string

		cmpb $0x41, %al # compare with 'A'
		jb .Lis_lower
		cmpb $0x5A, %al # compare with 'Z'
		ja .Lis_lower

		addb $0x20, %al # 'a' - 'A' = 61 - 41 = 20
		movb %al, (%rdi)

.Lis_lower:
	inc %rdi
	jmp .Lloop_start

.Lend_of_string:
	ret

/*
 * Remove any non-alphaabetic characters from the end of the
 * NUL-terminated character string pointed-to by w.
 *
 * C function prototype:
 *    void wc_trim_non_alpha(unsigned char *w);
 */
.globl wc_trim_non_alpha
wc_trim_non_alpha:
    # Find the end of the string
    subq $8, %rsp        # Adjust stack for alignment

    # Save %rsi since we'll use it, and it's a callee-saved register
    pushq %rsi

    # Make %rsi point to the start of the string, useful for ensuring we don't go beyond it
    movq %rdi, %rsi

.Lfind_end:
    movzbl (%rdi), %eax  # Load next byte
    testb %al, %al       # Check for null terminator
    jz .Ltrimming         # If found, begin trimming
    incq %rdi            # Move to the next character
    jmp .Lfind_end

.Ltrimming:
    decq %rdi            # Move back one character from null terminator

.Lcheck_char:
    # Ensure we're not at the start of the string
    cmpq %rsi, %rdi
    je .Ldone             # If we're at the start, we're done

    # Save registers
    pushq %rsi
    pushq %rdi

    # Call wc_isalpha to check the character
    movb (%rdi), %dil  # Load the character into dil
    call wc_isalpha
    testl %eax, %eax     # Test the result

    # Restore registers
    popq %rdi
    popq %rsi

    # If zero, not a letter
    jz .Lnot_a_letter

    # If it's a letter, we're done
    jmp .Ldone

.Lnot_a_letter:
    # Replace character with null terminator and move to the previous character
    movb $0, (%rdi)
    decq %rdi
    jmp .Lcheck_char

.Ldone:
    popq %rsi
    addq $8, %rsp
    ret


/*
 * Search the specified linked list of WordEntry objects for an object
 * containing the specified string.
 *
 * If a matching object is found, set the int variable pointed-to by
 * inserted to 0 and return a pointer to the matching object.
 *
 * If a matching object is not found, allocate a new WordEntry object,
 * set its next pointer to point to what head points to (i.e., so the
 * new object is at the head of the list), set the variable pointed-to
 * by inserted to 1, and return a pointer to the new node. Note that
 * the new node should have its count value set to 0. (It is the caller's
 * job to update the count.)
 *
 * C function prototype:
 *    struct WordEntry *wc_find_or_insert(struct WordEntry *head, const unsigned char *s, int *inserted);
 */
.globl wc_dict_find_or_insert
wc_dict_find_or_insert:
    
    subq $16 , %rsp             # Allocating space for variables
    pushq %r12 
    pushq %r13
    pushq %r14
    pushq %r15
    pushq %rbp
    pushq %rbx                  # Save registers
    
    
    movq %rdi, %r12             # store buckets pointer argument in register
    movl %esi, %r13d            # store num_buckets argument in register
    movq %rdx, %r14             # store string pointer argument in register
    
    
    movq %r14, %rdi             # load string pointer argument in rdi for hash function
    call wc_hash                           
    xor %edx, %edx              # clear argument register       
    
    movl %r13d, %ebx            # Copy num_buckets argument to register
    idiv %ebx                   # perform modulo division
    movl %edx, %ebp                     
    xor %r13, %r13              # clear argument register 
    movl %edx, %r13d            # move remainder to register
    imulq $8, %rbp              # find bucket offset
    
    
    pushq %r12                  # save register
    
    addq %rbp, %r12             # find bucket[index]
    movq (%r12), %rdi           # load bucket[index] into rdi for wc_find_or_insert func
    popq %r12                   # restore register
    movq %r14, %rsi             # load string argument into rsi for wc_find_or_insert func

    pushq %rbp                  # save register
    movq %rsp, %rbp             # align stack 
    subq $4, %rsp               # allocate space for inserted argument
    movl $0, (%rbp)             # initalize inserted argument
    movq %rbp, %rdx             # load stack frame address argument for wc_find_or_insert func
    
    call wc_find_or_insert
    movq %rax, %r15             # move returned NewEntry object to register
    cmpl $1, (%rbp)             # if inserted is 1, insert object to bucket[index]
    je .Linserted  

    addq $4, %rsp               # Realign stack pointer
    popq %rbp                   # Restore register
    jmp .Lnot_inserted

.Linserted:
    addq $4, %rsp               # Realign stack pointer
    popq %rbp                   # Restore register
    movq %r15, (%r12, %r13, 8)  # Update head of buckets[]

.Lnot_inserted:
    movq %r15, %rax             #  Move entry to return register


    popq %rbx                   # Restore registers
    popq %rbp
    popq %r15
    popq %r14
    popq %r13
    popq %r12
    addq $16 , %rsp             # Realign stack pointer

    ret


/*
 * Free all of the nodes in given linked list of WordEntry objects.
 *
 * C function prototype:
 *    void wc_free_chain(struct WordEntry *p);
 */
.globl wc_free_chain
wc_free_chain:

    pushq %r12              # Save registers
    pushq %r13
    
    movq %rdi, %r12         # move argument register 

.Lfree:

    cmpq $0, %r12           # check if pointer is not at end
    je .Lfree_done
    
    movq %r12, %rdi         # load pointer to argument register for free func
    movq WORDENTRY_NEXT_OFFSET(%r12), %r13      # find pointer-> next
    call free

    movq %r13, %r12         # move to next iteration with new node pointer
    jmp .Lfree


.Lfree_done:

    popq %r13               # Restore registers
    popq %r12

    ret


/*
vim:ft=gas:
*/