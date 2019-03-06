# senttech_camera
Omron Settech camera Linux drivers and supporting applications can be downloaded from the manufacturers website. 
https://sentech.co.jp/en/products/USB/mini.html

I bought a STC-MC152USB USB 2.0 camera and discovered that the driver stcam_dd did not compile on my Ubuntu Linux 18.04. 
This was due to a compilation error not recognising the kernel function init_timer.
The problem is found in function : int stcam_dd_mem_alloc_usb_data(struct stcam_st_dev_data \*ddata) 
of file *stcam_dd_mem.c

It turns out that from Kerner > 4.15 the timer API has changed and sentech had not updated their driver code.
I replaced the call *init_timers* with *timer_setup* in *stcam_dd_mem.c*. However, further changes were required as the *timer_list* structure was also updated and new way of passing and retrieving data to kernel timers was introduced. 
Check function *void stcam_dd_set_time_out(struct timer_list * tm)* in *stcam_dd_set.c* to see how data is retrieved with the new kernel.

I have updated the stcam_dd code to incorporate the new way kernel timers operate and wanted to share here to in case someone else runs into the same trouble. All you need to do is download the stcam_dd code and run make install in the stcam_dd directory. 

I have tested my camera with the sentech viewer provided in the package and it works fine. 

With the hope this helps somebody outhere,
Kostas

