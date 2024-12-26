// SPDX-License-Identifier: BSD-3-Clause

/*
 * Copyright (c) 2017, GlobalLogic Ukraine LLC
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by the GlobalLogic.
 * 4. Neither the name of the GlobalLogic nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY GLOBALLOGIC UKRAINE LLC ``AS IS`` AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL GLOBALLOGIC UKRAINE LLC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/ktime.h>
#include <linux/slab.h>   
#include <linux/moduleparam.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A simple Hello World Kernel Module");
MODULE_VERSION("1.0");


static unsigned int hello_count = 1; 
module_param(hello_count, uint, 0660);
MODULE_PARM_DESC(hello_count, "Number of times to print 'Hello, world!'");


struct hello_event {
    struct list_head list;
    ktime_t timestamp;
};

static LIST_HEAD(hello_list); 

static int __init hello_init(void)
{
    int i;
    struct hello_event *event;
    
   
    if (hello_count == 0 || (hello_count >= 5 && hello_count <= 10)) {
        pr_warn("Warning: hello_count is %u. Proceeding with caution.\n", hello_count);
    }
    else if (hello_count > 10) {
        pr_err("Error: hello_count is %u. Module will not load.\n", hello_count);
        return -EINVAL; 
    }

    
    for (i = 0; i < hello_count; i++) {
        pr_info("Hello, world!\n");

        
        event = kmalloc(sizeof(*event), GFP_KERNEL);
        if (!event) {
            pr_err("Failed to allocate memory for hello_event\n");
            return -ENOMEM;
        }

        
        event->timestamp = ktime_get();

       
        list_add_tail(&event->list, &hello_list);
    }

    return 0;
}


static void __exit hello_exit(void)
{
    struct hello_event *event, *tmp;

    
    list_for_each_entry_safe(event, tmp, &hello_list, list) {
        pr_info("Event timestamp: %lld ns\n", ktime_to_ns(event->timestamp));
        list_del(&event->list);
        kfree(event); 
    }
}

module_init(hello_init);
module_exit(hello_exit);
