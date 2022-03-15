'''
  flask http server
'''
from flask import request
from zgd import app
from models import ProductInfo

import json

import CommodityName

@app.route('/spinfo')
def success1():
   #return 'welcome %s' % name
   # args 取get参数 account = request.args.get('account')
   # form 取post参数 account = request.form.get('account')
   if request.method == 'GET':
        barcode = request.args.get('barcode', 'No barcode!')
        print(barcode)
        if len(barcode) < 13 and len(barcode) > 14:
            print(len(barcode))
            json.dumps({'ErrorCode':'1'}) # barcode ERROR
        result = ProductInfo.query.filter(ProductInfo.barcode == barcode).first()
        print(result)
        errorCode = 0
        if result:
            name = result.name
        else:
            name, errorCode = CommodityName.SearchCommodityName(barcode)
            commondity = ProductInfo(barcode = barcode, name = name)
            commondity.save()
        print(name)
        retstr = {'ErrorCode':str(errorCode),"CommodityName":name,"barcode":barcode}
        return json.dumps(retstr)
   else:
       return json.dumps({'ErrorCode':'1'})


if __name__ == '__main__':
  print('##test##')