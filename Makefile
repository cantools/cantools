# create release and upload to pypi

all:
	python setup.py sdist
	python setup.py bdist_wheel --universal
	twine upload dist/*
