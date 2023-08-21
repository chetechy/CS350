# CS350
Emerging Systems Architectures &amp; Technologies
# Summarize the project and what problem it was solving.
The final project was to develop a smart thermostat protype using the TI board for SysTec. The prototype used the TMP006 temperature sensor to read the room temperature (via I2C), the LEDs had to indicate the output to the thermostat where LED on = heat on (via GPIO), two buttons had to increase and decrease the set temperature (via GPIO interrupt), and the UART had to simulate the data being sent to the server.

# What did you do particularly well?
I felt like I figured out the configurations pretty well, and was able to use the open declaration feature to my advantage. It helped me understand several functions and what parameters should be passed.

# Where could you improve?
My biggest issue time management. I felt like I was always pushing way harder towards the end with less time to figure out issues I may have been having in my code. This left me with very little room to ask for assistance in a timely manner.

# What tools and/or resources are you adding to your support network?
This was my first time ever working with an embedded system. I really enjoyed working with the Code Composer Studio, it was very similar to Eclipse.  Then, the biggest resource that helped me was our ZyBooks book and TI pdfs.

# What skills from this project will be particularly transferable to other projects and/or course work?
Just doing proper research on how to correctly build certain models, such as the state machines and task schedulers. Also, looking at example codes and figuring out how I can implement them into my program.

# How did you make this project maintainable, readable, and adaptable?
For the Thermostat program, I had already utilized a lot of recycled code from the past milestones. So, this program is very adaptable. Also, I made sure to comment a lot of key lines to tell readers what they are used for. Lastly, I tried to keep the code as simple as possible, so it could be easily read and maintained.
