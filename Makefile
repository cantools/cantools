test:
	python2 setup.py test
	python3 setup.py test
	env PYTHONPATH=. python2 examples/hello_world.py
	env PYTHONPATH=. python3 examples/hello_world.py
	env PYTHONPATH=. python3 examples/dbc_io/main.py
	codespell -d $$(git ls-files | grep -v the_homer\.kcd)

release-to-pypi:
	python setup.py sdist
	python setup.py bdist_wheel --universal
	twine upload dist/*
