'''
 数据库 操作
'''

from zgd import db

class ProductInfo(db.Model):
    __tablename__ = "Productinfo"
    id = db.Column(db.Integer , primary_key=True , autoincrement=True)
    barcode = db.Column(db.String(128) , nullable=False)
    name = db.Column(db.String(128) , nullable=True)
    img_path = db.Column(db.String(1024) , nullable=True)

    def save(self):
        db.session().add(self)
        db.session().commit()

    def delete(self):
        db.session().delete(self)
        db.session().commit()

    def update(self):
        db.session().commit()

if __name__ == '__main__':
    print('###test###')