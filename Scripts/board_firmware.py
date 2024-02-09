# Copying bootloader output files in his folder.
# Для того чтобы скрипт работал правильно нужно 
# в том же 'env' обьявить параметр board_firmware_dir
# в котором указан путь и имя файла куда будет скопирована прошивка.
# Пример:
# [env:prog]
# board_firmware_dir = Prog/Firmware/firmware.bin

Import('env')

import shutil
import os,sys
from os.path import join, exists, split, splitext, isfile

# Copy output files to specifed directory.



def before_upload(source, target, env):
    print(f"before_upload {source[0]} ---------------")

def before_build(source, target, env):
    print(f"before_build {source[0]} ---------------")

def before_checksize(source, target, env):
    print(f"before_checksize {source[0]} ---------------")

try:
	target_list = env.GetProjectOption('board_firmware.file').split()
except:
	pass
else:
	for target in target_list:
		target_ext = splitext(target)[1]
		source_file = '$BUILD_DIR\\${PROGNAME}' + target_ext
		target_file = target.replace('/','\\')


		# Удаление файла прошивки перед созданием.
		if isfile(target_file):
			delete_file = env.Action(
				f'del /Q /F {target_file} > nul',
				f'Removing {target_file}'
			)
		else:
			delete_file = env.Action(
				f'echo File not found {target_file}',
				f''
			)

		env.AddPreAction(source_file, delete_file)
		#env.AddPreAction(source_file, before_build)

		# Копирование файла после создания.
		copy_file = env.Action(
			f'copy {source_file} {target_file} > nul',		# '> nul' чтобы скрыть вывод.
			f'Copying {source_file} to {target_file}'
		)
		env.AddPostAction(source_file, copy_file)

		# Удаление файла. # добавить скрипт который будет удалять все файлы прошивок.
		""" cmd_str = ' '.join(['del', target_file])
		print(cmd_str)

		out_str = ' '.join(['Removed', target_file])
		print(out_str)

		del_file = env.Action(cmd_str, out_str)
		env.AddPostAction('clean', del_file) """



		if target_ext == '.srec':
			source_file = '$BUILD_DIR\\${PROGNAME}.elf'		# Исходный файл.
			target_file = '$BUILD_DIR\\${PROGNAME}' + target_ext	# Целевой файл.

			# Действие.
			# Определяет каким образом будет создаваться файл.
			act = env.Action(
				f'$OBJCOPY -O srec {source_file} {target_file}',		# Команда командной строки.
				f'Building {target_file}'								# Текст который будет выводиться в командную строку.
			)


			# Это решение работает, но только при нажатии кнопки "Build" при нажатии
			# кнопки "Upload" файл .srec не генерируется.
			# Билдер.
			# Создает одноразовый билдер.
			cmd = env.Command(target_file, source_file, act)

			# По умолчанию.
			# Добавляет в перечень целевых файлов, которые создаются по умолчанию.
			env.Default(cmd)

			# Добавляем действие которое должно выполниться после создания "source_file".
			env.AddPostAction(source_file, act)

# Prebuild specifed env before buildprog.
try:
	pre_build_envs = env.GetProjectOption('board_firmware.prebuild').split()
except:
	pass
else:
	for item in pre_build_envs:
		if len(item) > 0:
			env.AddPreAction(
				'$BUILD_DIR\\${PROGNAME}.elf',
				env.VerboseAction(f"pio run -s -e {item}", f"Prebuild {item}...")
			)


# Pass linker flags.
def add_ldflag(ldflags):
	for flag in ldflags:
		env.Append(LINKFLAGS=[flag])

try:
	user_ldflags = env.GetProjectOption('board_ldflags').split()
except:
	pass
else:
	if len(user_ldflags) > 0:
		add_ldflag(user_ldflags)


# Print memory info.
source_file = '$BUILD_DIR\\${PROGNAME}.elf'
print_memory_info = env.Action(
	f'arm-none-eabi-size {source_file}',		# '> nul' чтобы скрыть вывод.
	f'Memory size'
)
env.AddPostAction(source_file, print_memory_info)