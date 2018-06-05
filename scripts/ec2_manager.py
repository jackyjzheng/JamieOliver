import boto3
from subprocess import call


ec2 = boto3.resource('ec2', region_name="us-east-1")

# For the web server
instance = ec2.create_instances(
    ImageId='ami-a4dc46db',
    MinCount=1,
    MaxCount=1,
    SecurityGroupIds=[
    	'sg-07aea7b9c198d6fbc'
    ],
    KeyName='test',
    InstanceType='t2.micro')

currInstance = instance[0]
currInstance.wait_until_running()
currInstance.load()
dnsName = currInstance.public_dns_name

#cmd = 'scp -i /Users/dennisfong/test.pem /Users/dennisfong/Desktop/Projects/JamieOliver/rest-server/target/gs-rest-service-0.1.0.jar ' + 'ubuntu@' + dnsName + ':~'
cmd = 'scp -i /Users/dennisfong/test.pem /Users/dennisfong/test.txt ' + 'ubuntu@' + dnsName + ':~'

call(cmd.split())

# Worker 1
instance = ec2.create_instances(
    ImageId='ami-a4dc46db',
    MinCount=1,
    MaxCount=1,
    SecurityGroupIds=[
    	'sg-07aea7b9c198d6fbc'
    ],
    KeyName='test',
    InstanceType='t2.micro')

currInstance = instance[0]
currInstance.wait_until_running()
currInstance.load()
dnsName = currInstance.public_dns_name

cmd = 'scp -i /Users/dennisfong/test.pem /Users/dennisfong/test.txt ' + 'ubuntu@' + dnsName + ':~'
call(cmd.split())

image = currInstance.create_image(
	Name='Ramsey-Client1')
imageId = image.image_id
image.wait_until_exists()
print(imageId)

# Worker 2
instance = ec2.create_instances(
    ImageId=imageId,
    MinCount=1,
    MaxCount=1,
    InstanceType='t2.micro')

# # Worker 3
# instance = ec2.create_instances(
#     ImageId='ami-a4dc46db',
#     MinCount=1,
#     MaxCount=1,
#     InstanceType='t2.micro')

# # Worker 4
# instance = ec2.create_instances(
#     ImageId='ami-a4dc46db',
#     MinCount=1,
#     MaxCount=1,
#     InstanceType='t2.micro')

# # Worker 5
# instance = ec2.create_instances(
#     ImageId='ami-a4dc46db',
#     MinCount=1,
#     MaxCount=1,
#     InstanceType='t2.micro')

# # Worker 6
# instance = ec2.create_instances(
#     ImageId='ami-a4dc46db',
#     MinCount=1,
#     MaxCount=1,
#     InstanceType='t2.micro')

# # Worker 7
# instance = ec2.create_instances(
#     ImageId='ami-a4dc46db',
#     MinCount=1,
#     MaxCount=1,
#     InstanceType='t2.micro')

# # Worker 8
# instance = ec2.create_instances(
#     ImageId='ami-a4dc46db',
#     MinCount=1,
#     MaxCount=1,
#     InstanceType='t2.micro')

# # Worker 9
# instance = ec2.create_instances(
#     ImageId='ami-a4dc46db',
#     MinCount=1,
#     MaxCount=1,
#     InstanceType='t2.micro')

# # Worker 10
# instance = ec2.create_instances(
#     ImageId='ami-a4dc46db',
#     MinCount=1,
#     MaxCount=1,
#     InstanceType='t2.micro')
