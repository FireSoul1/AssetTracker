# project/models.py


import datetime


class User:

    def __init__(self, email, password, admin=False):
        self.email = email
        self.password = password
        self.registered_on = datetime.datetime.now()
        self.admin = admin

    def is_authenticated(self):
        return True

    def is_active(self):
        return True

    def is_anonymous(self):
        return False

    def get_id(self):
        return self.id

    def __repr__(self):
        return '<User {0} {1} {2}>'.format(self.email, self.password, self.registered_on)
