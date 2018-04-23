################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
third_party/FreeRTOS/Source/portable/CCS/ARM_CM4F/port.obj: C:/ti/TivaWare_C_Series-2.1.4.178/third_party/FreeRTOS/Source/portable/CCS/ARM_CM4F/port.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccs730/ccsv7/tools/compiler/ti-cgt-arm_16.9.7.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me -Ooff --include_path="C:/ti/ccs730/ccsv7/tools/compiler/ti-cgt-arm_16.9.7.LTS/include" --include_path="D:/GitHub/ECEN-5013/Project2/Project2_FreeRTOS_TIVA" --include_path="C:/ti/TivaWare_C_Series-2.1.4.178" --include_path="C:/ti/TivaWare_C_Series-2.1.4.178/third_party" --include_path="C:/ti/TivaWare_C_Series-2.1.4.178/third_party/FreeRTOS/Source/include" --include_path="C:/ti/TivaWare_C_Series-2.1.4.178/third_party/FreeRTOS" --include_path="C:/ti/TivaWare_C_Series-2.1.4.178/third_party/FreeRTOS/Source/portable/CCS/ARM_CM4F" --advice:power="all" --define=ccs="ccs" --define=DEBUG --define=TARGET_IS_TM4C123_RB1 --define=PART_TM4C1294NCPDT -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --gen_func_subsections=on --abi=eabi --ual --preproc_with_compile --preproc_dependency="third_party/FreeRTOS/Source/portable/CCS/ARM_CM4F/port.d_raw" --obj_directory="third_party/FreeRTOS/Source/portable/CCS/ARM_CM4F" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

third_party/FreeRTOS/Source/portable/CCS/ARM_CM4F/portasm.obj: C:/ti/TivaWare_C_Series-2.1.4.178/third_party/FreeRTOS/Source/portable/CCS/ARM_CM4F/portasm.asm $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccs730/ccsv7/tools/compiler/ti-cgt-arm_16.9.7.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me -Ooff --include_path="C:/ti/ccs730/ccsv7/tools/compiler/ti-cgt-arm_16.9.7.LTS/include" --include_path="D:/GitHub/ECEN-5013/Project2/Project2_FreeRTOS_TIVA" --include_path="C:/ti/TivaWare_C_Series-2.1.4.178" --include_path="C:/ti/TivaWare_C_Series-2.1.4.178/third_party" --include_path="C:/ti/TivaWare_C_Series-2.1.4.178/third_party/FreeRTOS/Source/include" --include_path="C:/ti/TivaWare_C_Series-2.1.4.178/third_party/FreeRTOS" --include_path="C:/ti/TivaWare_C_Series-2.1.4.178/third_party/FreeRTOS/Source/portable/CCS/ARM_CM4F" --advice:power="all" --define=ccs="ccs" --define=DEBUG --define=TARGET_IS_TM4C123_RB1 --define=PART_TM4C1294NCPDT -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --gen_func_subsections=on --abi=eabi --ual --preproc_with_compile --preproc_dependency="third_party/FreeRTOS/Source/portable/CCS/ARM_CM4F/portasm.d_raw" --obj_directory="third_party/FreeRTOS/Source/portable/CCS/ARM_CM4F" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


