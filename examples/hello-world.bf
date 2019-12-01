# Save the current memory address and then write the letter H
( ^48
# Move right and write the rest of the letters
> ^65
> ^6C
> ^6C
> ^6F
> ^20
> ^57
> ^6F
> ^72
> ^64
> ^21
> ^0A
# reset memory adress and start a loop
)[
	# Print the current item
	.
	# Move right
	>
# Loop will continue till the pointer no longer points at the letters since the next byte will be 0x0 and the loop condition then is broken. (we're printing a null terminated string)
])/
# After ending the loop we go back to the first memory adress and remove the saved memory adress from the list
