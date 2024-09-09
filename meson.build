project('onegin', 'c',
  version : '0.1',
  default_options : ['warning_level=3'])

incdir = include_directories('include')

# Setup arrays to collect filenames into
sources = []
tests = []

# Call setup for subdirs
# subdir('include') # No file for now, because it is not needed
subdir('src')
subdir('test')

# Main executable

exe = executable(
  'onegin',
  [main_file] + sources,
  include_directories : incdir,
  install : true
)

# Executable for tests
# NOTE: maybe add options to specify which groups to run?

tests = executable(
  'tests',
  tests + sources,
  include_directories : incdir,
)

test('normal-tests', tests)
