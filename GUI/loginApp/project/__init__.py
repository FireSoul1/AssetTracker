# project/__init__.py
import os
from random import *
from flask import Flask, request, jsonify, session, render_template
from models import User
from passlib.hash import pbkdf2_sha256
from sqlalchemy import inspect, create_engine
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy.orm import sessionmaker

# config
num = 2
app = Flask(__name__, template_folder='./static')
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
    print("Registers "+json_data['email']+" "+json_data['password'])
    user = User(email=json_data['email'], password=hashPassword(json_data['password']))
    if not checkIfRegistered(user):
        print("registered")
        status = 'This user is already registered'
    else:
        registerUser(user)
        status = 'success'
    return jsonify({'result': status})

@app.route('/api/login', methods=['POST'])
def login():
    print('logging in')
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
    print('status: ' + str(status))
    return jsonify({'result': status})


@app.route('/api/logout')
def logout():
    session.pop('logged_in', None)
    return jsonify({'result': 'success'})

@app.route('/api/status')
def status():
    print(request)
    if session.get('logged_in'):
        if session['logged_in']:
            print('session: True')
            return jsonify({'status': True})
    else:
        print('session: False')
        return jsonify({'status': False})

@app.route('/api/show')
def triggerShow():
    global show
    show = not show
    return jsonify({'vis': show})

@app.route('/api/login/<user>')
def get_time(user):
    return jsonify({'username': user})

@app.route('/<devid>/picture')
def get_picture(devid):
    #get the new image in on the system
    output = conn.execute("SELECT \'"+devid+"\', picpath FROM images;")
    value = output[0][1]
    return jsonify({'picpath': value})

@app.route('/<devid>/<user>/active')
def activate_user(devid, user):
    #insert the device to table
    #username = (user.split("@")[0]).replace(".","_")
    print("  ACTIVATING USER: "+user+" "+str(devid))
    output = conn.execute("SELECT locked FROM active_users WHERE uid="+user+" AND devid="+devid+";")
    for value in output:
        outss = conn.execute("SELECT devid, uid FROM shares WHERE devid="+str(devid)+";")
        if(value[0] == 0): #unlocked
            print('set 1')
            for vals in outss:
                conn.execute("UPDATE active_users SET locked=1 WHERE uid="+str(vals[1])+" AND devid="+str(devid)+";")
        else: #locked
            print('set to 0')
            #check that other user that can access are active
            for vals in outss:
                #check that active the userId
                new11 = conn.execute("SELECT uid FROM active_users WHERE uid="+str(vals[1])+";").fetchall()
                if(len(new11) <= 0): #not in active_user
                    value = -1
                    print("Nope")
                    missed = conn.execute("SELECT name FROM users WHERE id="+str(vals[1])+";").fetchall()
                    valR  = missed[0][0]
                    conn.execute("INSERT INTO active_users (uid,devid,locked) VALUES ("+user+","+str(devid)+", 1);")
                    return jsonify({'status': value, 'others': valR})
            print("UNLOCKINGGGG")
            #everyone is active. Time to connect. Unlock!
            for vals in output:
                conn.execute("UPDATE active_users SET locked=0 WHERE uid="+str(vals[1])+" AND devid="+str(devid)+";")
    if(len(output) == 0):
        conn.execute("INSERT INTO active_users (uid,devid,locked) VALUES ("+user+","+str(devid)+", 1);")
    value = True
    return jsonify({'status': value})

@app.route('/api/<user>/get_devices')
def get_devices(user):
    #get current UserID
    username = (user.split("@")[0]).replace(".","_")
    output = conn.execute("SELECT \'"+username+"\', id FROM users;").fetchall()
    value = output[0][1]
    #get deivce that fit to current User Id
    output = conn.execute("SELECT uid, devid FROM shares;")
    devices = []
    for val in output:
        if(val[0] == value):
            if( not (val[1] in devices) ):
                print(val[1], end=" ")
                devices.append(val[1])
    print(" ")
    return jsonify( {'devices': devices, 'userid':value} )

@app.route('/<image>', methods=['GET'])
def retrieve_static_res(image):
    if(len(image.split('.')) > 1 ):
        return app.send_static_file("/tmp/image/"+image)
    return app.send_static_file('index.html')

#GPS Routes
@app.route('/<lat>/<lng>', methods=['GET'])
def showMap(lat, lng):
    return render_template('map.html', lat=lat, lng=lng, key='AIzaSyAK9w5gyw6vMO-ec0Bbt1meRls7O-LjuUE')

@app.route('/<devid>/location', methods=['GET'])
def get_loc(devid):
    test = conn.execute("SELECT "+devid+", locationX, locationY FROM device_locs;")
    valX = ""
    valY = ""
    for val in test:
        print(str(val[0])+" "+val[2]+" ")
        if(str(val[0]) == (""+devid)):
            print("^^ Selected")
            valX = val[1]
            valY = val[2]
            print("  "+str(valX)+" "+valY+" ")
            return jsonify({ 'Xloc': valX, 'Yloc': valY })
    return jsonify({ 'Xloc': valX, 'Yloc': valY })

def hashPassword(password):
    return pbkdf2_sha256.encrypt(password, rounds=200000, salt_size=16)

def verifyPassword(password, challenge):
    return pbkdf2_sha256.verify(password, challenge)

def checkIfRegistered(user):
    username = (user.email.split("@")[0]).replace(".","_")
    test = conn.execute("SELECT \'"+username+"\' FROM auth_users;").fetchall()
    print('[%s]' % test[0][0])
    if(len(test) > 0):
        print("Check Auth")
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
    global num;
    userId = num;
    num += randint(2, num+1)
    conn.execute("INSERT INTO users (id, name) VALUES (\'"+str(userId)+"\',\'"+username+"\');")
def validate(user):
    userName = (user.email.split("@")[0]).replace(".","_")
    print(userName)
    output = conn.execute("SELECT \'"+userName+"\', passwords FROM auth_users;").fetchall()
    if(len(output) > 0 ): #the user exists
        test = dict(output)
        #print(test)
        #compare passwords
        password = test[userName]
        #print("Validating: "+userName+" "+password)
        return True
    return False;


if __name__ == '__main__':
    app.debug = True
    app.run(host='0.0.0.0', port=5000)
