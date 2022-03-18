
struct CMF_tag {
   unsigned int my;
};

typedef struct{
    unsigned int array_base;                               
    struct CMF_tag *control[ 2 ];  
    unsigned int ctl_program[ 1 ];      
    unsigned int ctl_erase[ 1 ];          
    unsigned short max_program_pulses;                       
    unsigned short max_erase_pulses;

    unsigned char  paws_program[ 1 ];     
    unsigned char  paws_erase[ 1 ];         
    unsigned short paws_program_pulses[1];   
    unsigned short paws_erase_pulses[1];     

}tCMF_DESCRIPTOR;
