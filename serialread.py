import serial
import re
import psycopg2
from psycopg2 import Error
import keyboard
import subprocess
import os

#Global Variables
ser = 0
def insert_data(ime, prezime,jmabg):
    try:
        proc = subprocess.Popen('heroku config:get DATABASE_URL -a intense-ravine-30063', stdout=subprocess.PIPE, shell=True)
        db_url = proc.stdout.read().decode('utf-8').strip() + '?sslmode=require'

        connection = psycopg2.connect(db_url)

        cursor = connection.cursor()

        postgres_insert_query = """ INSERT INTO student (ime, prezime, jmbag) VALUES (%s,%s,%s)"""

        record_to_insert = (ime, prezime, jmabg)

        cursor.execute(postgres_insert_query, record_to_insert)

        connection.commit()
        count = cursor.rowcount()
    except (Exception, psycopg2.Error) as error:
        print("ej", error)
    finally:
        if(connection):
            cursor.close()
            connection.close()

def select_data(jmbag):
    try:
       proc = subprocess.Popen('heroku config:get DATABASE_URL -a intense-ravine-30063', stdout=subprocess.PIPE, shell=True)
       db_url = proc.stdout.read().decode('utf-8').strip() + '?sslmode=require'

       connection = psycopg2.connect(db_url)

       cursor = connection.cursor()

       postgreSQL_select_Query = """select * from student where jmbag='%s'"""%jmbag
       #record_to_select = (jmbag)

       cursor.execute(postgreSQL_select_Query)

       rows = cursor.fetchall()

       return len(rows)
       #print(len(rows))
    except (Exception, psycopg2.Error) as error :
        print ("Error while fetching data from PostgreSQL", error)
    finally:
        #closing database connection.
        if(connection):
            cursor.close()
            connection.close()


#Function to Initialize the Serial Port
def init_serial():
    global ser          #Must be declared in Each Function
    ser = serial.Serial(port='COM6', baudrate=9600, bytesize=serial.EIGHTBITS, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE)



def main():
    #Call the Serial Initilization Function, Main Program Starts from here
    init_serial()
    while 1:

        bytes = ser.readline().decode('utf-8')  #Read from Serial Port

        if(len(bytes) == 84):
            byte = re.split('\W+', bytes)
            prezime = byte[2]
            ime = byte[3]
            oib = byte[4]
            jmbag = byte[5]
            print("Accepted")
            if select_data(jmbag) == 0:
                insert_data(ime, prezime, jmbag)

        else:
            print("Denied")


if __name__ == "__main__":
    main()
