# project/__init__.py
import os
from random import *
from flask import Flask, request, jsonify, session
from models import User
from passlib.hash import pbkdf2_sha256
import pyrebase
from sqlalchemy import inspect, create_engine
from sqlalchemy.ext.declarative import declarative_base


# config

app = Flask(__name__)
app.config['SECRET_KEY'] = "hello"
filename = 'firebase.txt'
#firebase
#firebase = pyrebase.initialize_app(config)
#current User
user = None
userId = -100

#SQLAlchemy
engine = create_engine('mysql+pymysql://n4q1a6kczuonf5l8:jsgsb3vqi3xvty8f@i943okdfa47xqzpy.cbetxkdyhwsb.us-east-1.rds.amazonaws.com:3306/ffj056kwbtsekz5v')
conn = engine.connect()

#Verify that all the Tables have the right columns
# Base = declarative_base()
# inspector = inspect(engine)
# for table_name in inspector.get_table_names():
#     print("Table: %s" % table_name)
#     for column in inspector.get_columns(table_name):
#         print("%s " % column['name'], end=" ")
#     print("\n")
# config = os.environ.get("config")
# print(config)



# routes
@app.route('/')
def index():
    return app.send_static_file('index.html')


@app.route('/api/register', methods=['POST'])
def register():
    json_data = request.json
    user = User(email=json_data['email'], password=hashPassword(json_data['password']))
    if checkIfRegistered(user):
        #print("registered")
        status = 'This user is already registered'
    else:
        registerUser(user)
        status = 'success'
    return jsonify({'result': status})

@app.route('/api/login', methods=['POST'])
def login():
    json_data = request.json
    user = User(email=json_data['email'], password=json_data['password'])
    if checkIfRegistered(user) and validate(user):
        session['logged_in'] = True
        status = True
        #get the UserId
        username = (user.email.split("@")[0]).replace(".","_")
        output = conn.execute("SELECT \'"+username+"\', id FROM users;").fetchall()
        global userId
        userId = output[0][1]
        print("LoginL "+str(userId))
    else:
        status = False;
    user.password = hashPassword(user.password)
    return jsonify({'result': status})


@app.route('/api/logout')
def logout():
    session.pop('logged_in', None)
    return jsonify({'result': 'success'})


@app.route('/api/status')
def status():
    if session.get('logged_in'):
        if session['logged_in']:
            return jsonify({'status': True})
    else:
        return jsonify({'status': False})

@app.route('/api/show')
def triggerShow():
    global show
    show = not show
    return jsonify({'vis': show})

@app.route('/api/login/<user>')
def get_time(user):
    return jsonify({'username': user})

@app.route('/api/<user>/get_devices')
def get_devices(user):
    #get deivce that fit to current User Id
    output = conn.execute("SELECT uid, devid FROM shares;").fetchall()
    devices = []
    for val in output:
        if(val[0] == userId):
            print(val[1], end=" ")
            if( not (val[1] in devices) ):
                devices.append("Device "+str(val[1]))
    print(" ")
    print(str(userId)+ ": "+str(len(devices)) )
    return jsonify({'devices':devices})

def hashPassword(password):
    return pbkdf2_sha256.encrypt(password, rounds=200000, salt_size=16)

def verifyPassword(password, challenge):
    return pbkdf2_sha256.verify(password, challenge)

def checkIfRegistered(user):
    username = (user.email.split("@")[0]).replace(".","_")
    test = conn.execute("SELECT \'"+username+"\', passwords FROM auth_users;").fetchall()
    print("Check Auth")
    #print('[%s]' % ', '.join(map(str, test)))
    if(len(test) > 0):
        return True
    return False

#Register
def registerUser(user):
    #add the User to the DB

    username = (user.email.split("@")[0]).replace(".","_")
    passsalt = user.password
    print("Adding to Auth")
    conn.execute("INSERT INTO auth_users (username,passwords) VALUES ( \'"+username+"\',\'"+passsalt+"\');")
    print("Adding to UserId")
    userId = random(1,10000);
    conn.execute("INSERT INTO users (id, name) VALUES (\'"+userId+"\',\'"+username+"\');")


def validate(user):
    #TODO check the DB for Username and Password
    userName = (user.email.split("@")[0]).replace(".","_")
    output = conn.execute("SELECT \'"+userName+"\', passwords FROM auth_users;").fetchall()
    if(len(output) > 0 ): #the user exists
        test = dict(output)
        #print(test)
        #compare passwords
        password = test[userName]
        #print("Validating: "+userName+" "+password)
        return verifyPassword(user.password, password)
    return False;
    # f = open(filename, 'r+')
    # lines = f.readlines()
    # for l in lines:
    #     if user.email in l:
    #         arr = l.split(' ')
    #         return verifyPassword(user.password, arr[2])
    # return False
if __name__ == '__main__':
    app.debug = True
    app.run()
