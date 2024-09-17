project('onegin', 'c',
  version : '0.1',
  default_options : ['warning_level=3'])


istd_proj = subproject('istd')
istd_dep = istd_proj.get_variable('dep')

incdir = include_directories('include')

# Setup arrays to collect filenames into
sources = []
test_files = []

# Call setup for subdirs
# subdir('include') # No file for now, because it is not needed
subdir('src')
subdir('test')

# Main executable

exe = executable(
  'onegin',
  [main_file] + sources,
  include_directories : incdir,
  dependencies : istd_dep,
  install : true
)

# Executable for tests
# NOTE: maybe add options to specify which groups to run?

tests = executable(
  'test_exe',
  test_files + sources,
  dependencies : istd_dep,
  include_directories : incdir,
)

test('normal-tests', tests)
