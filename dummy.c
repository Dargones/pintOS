
struct thread *curr_thread = thread_current();
  if(!list_empty(&curr_thread->donation_list)){//if there are donations
    printf("list isn't empty\n");
    printf("list size: %d\n",list_size(&curr_thread->donation_list));
    //pop the first element of the thread's donation list
    struct list_elem *donation = list_begin(&curr_thread->donation_list);
    //get the doner of the donation
    struct thread *doner_thread = list_entry(donation, struct thread, donation_list_elem);
    //set priority of current thread to donator's priority (can't set priority lower)
    if (doner_thread->priority>curr_thread->priority){
      curr_thread->priority = doner_thread->priority;
    }
  }else{
    curr_thread->priority = new_priority;
  }

int update_actual_priority(struct *thread thread_to_update){
  if(!list_empty(&curr_thread->donation_list)){
    //look at the first element of the thread's donation list
    struct list_elem *donation = list_begin(&thread_to_update->donation_list)
    //get the doner of the donation
    struct thread *doner_thread = list_entry(donation, struct thread, donation_list_elem);
    //set priority of current thread to donator's priority (can't set priority lower)
    if (doner_thread->priority>&thread_to_update->base_priority){
      &thread_to_update->priority = doner_thread->priority;
    }else{
      &thread_to_update->priority = &thread_to_update->base_priority;
    }
  }else{
    &thread_to_update->priority = &thread_to_update->base_priority;
  }
  return 
}