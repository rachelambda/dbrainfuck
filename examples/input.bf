# Set the amount of chars we want to read
^0A
# Save the adress and start a loop
([
	# Jump as many steps forward as the current adress (our starting number) and take an input
	} ,
	# Jump back to the beggining and decrese the value of the byte, making it so that we jump one less byte forwards next time, and eventually makes us break the loop
	) -
# After ending the loop we jump back and remove the memory adress from the list
])
# Move to start of null terminated string and print it
>
[
	>
	.
]
